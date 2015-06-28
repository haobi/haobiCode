//#include <unistd.h>
#ifndef SYSTEM_WIN

#include "mysem.h"

#define SET_ERROR_INFO \
	do { \
		m_LINE = __LINE__; \
		sprintf( m_FILE,"%s",__FILE__ ); \
		m_errno = errno; \
	} while(0)

#define RS_NO_ERROR \
	do { \
		m_LINE = __LINE__; \
		sprintf( m_FILE,"%s",__FILE__ ); \
		m_errno = 0; \
	} while(0)

TSem::TSem() {

	m_key = -1;
	m_semid = -1;
	m_errno = 0;
	m_LINE = 0;
	m_ptr = (struct sembuf *)calloc( sizeof(struct sembuf),1 );
	memset( m_FILE,0,MAX_BUF );
	memset( m_errmsg,0,MAX_BUF );
}

TSem::~TSem() {
	free(m_ptr);
}

bool TSem::sem_create( key_t key,int total,int total_array[] ) {

	int semid;
	bool flag = true;
	if ( -1 != ( semid = semget( key,total,0666 | IPC_CREAT | IPC_EXCL ) ) ) {

		struct sembuf *sb = new struct sembuf[total];
		for ( int i = 0; i < total; i++ ) {

			sb[i].sem_num = i;
			sb[i].sem_op = total_array[i];
			sb[i].sem_flg = 0;
		}

		if ( -1 == semop( semid,sb,total ) ) {
			SET_ERROR_INFO;
			return false;
		}

		RELEASE_SZ(sb);

		m_semid = semid;
		m_key = key;
	}
	else {
		SET_ERROR_INFO;
		flag = false;
	}
	return flag;
}

bool TSem::sem_get( key_t key ) {

	bool flag = false;
	if ( -1 == ( m_semid = semget( key,0,IPC_EXCL ) ) ) {
		SET_ERROR_INFO;
		return false;
	}
	// 等待创建进程把信号量创建完成
	for ( int i = 0; i < MAX_TRIES; i++ ) {
		if ( 0 != get_sem_otime() ) {
			flag = true;
			break;
		}
		usleep(10);
	}

	m_key = key;

	if ( !flag ) {
		m_semid = -1;
		SET_ERROR_INFO;
	}
	return flag;
}
/*
 * 占用指定锁的资源的数量
 * 参数：
 * no : 锁的位置
 * count : 占用的资源的数目
 * */
bool TSem::sem_lock(int no,int count) {

	if ( no < 0 || count < 1 )
		RS_NO_ERROR;
//	return set_op(-1 * count,no,SEM_UNDO);
	return set_op(-1 * count,no,count == 0 ? 0 : SEM_UNDO);
}

bool TSem::sem_try_lock(int no,int count) {

	if ( no < 0 || count < 1 )
		RS_NO_ERROR;
//	return set_op(-1 * count,no,IPC_NOWAIT | SEM_UNDO);
	return set_op(-1 * count,no,count == 0 ? 0 : SEM_UNDO);
}

bool TSem::sem_unlock(int no,int count) {

	if ( no < 0 || count < 1 )
		RS_NO_ERROR;
//	return set_op(count,no,SEM_UNDO);
	return set_op(-1 * count,no,count == 0 ? 0 : SEM_UNDO);
}

bool TSem::set_op( int op,int no,int flag ) {

	bool f = true;

//	struct sembuf *ptr = (struct sembuf *)calloc( sizeof(struct sembuf),1 );
	m_ptr->sem_num = no;
	m_ptr->sem_op = op;
	m_ptr->sem_flg = flag;
	
redo:
	if ( -1 == semop( m_semid,m_ptr,1 ) ) {
		if ( EINTR == errno )
			goto redo;
		else {
			SET_ERROR_INFO;
			f = false;
		}
	}
//	free( ptr );
	return f;
}

bool TSem::sem_delete() {

	if ( -1 == semctl( m_semid,0,IPC_RMID,NULL ) ) {
		SET_ERROR_INFO;
		return false;
	}
	return true;
}
/*
 * 参数:
 * no : 获取指定锁的资源数量
 * */
int TSem::get_sem_count(int no) {

	int rs;
	union semun arg;
	if ( -1 == ( rs = semctl( m_semid,no,GETVAL,&arg ) ) )
		SET_ERROR_INFO;
	return rs;
}

time_t TSem::get_sem_otime() {

	time_t t = 0;
	union semun arg;		
	struct semid_ds seminfo;
	arg.buf = &seminfo;
	if ( -1 != semctl( m_semid,0,IPC_STAT,arg ) )
		t = arg.buf->sem_otime;
	return t;
}
/*
 * 返回：获取锁的数量
 * */
int TSem::get_sem_total_count() {

	union semun arg;
	struct semid_ds seminfo;
	arg.buf = &seminfo;
	if ( -1 != semctl( m_semid,0,IPC_STAT,arg ) ) 
		return arg.buf->sem_nsems;
	return -1;
}

bool TSem::print_info() {

	int nsems = get_sem_total_count();
	printf("************* sem info *************\n");
	printf("pid : %d\n",semctl( m_semid,0,GETPID ));
	for ( int i = 0; i < nsems; i++ )
		printf("semval[%d] : %d\n",i,get_sem_count(i));
	printf("************************************\n");
	return true;
}

const char* TSem::get_err_msg() {

	sprintf( m_errmsg,"[FILE:%s][LINE:%d] error[%d] : %s",m_FILE,m_LINE,errno,strerror(m_errno) );
	return m_errmsg;
}

#endif
