#include "myDef.h"
#include "myLock.h"
//-----------------------------------------------------------
//					简单的线程锁
//-----------------------------------------------------------
#define MAX_TRIES 100

TThreadLock::TThreadLock() {
#ifdef SYSTEM_WIN
	m_hMutex = NULL;
#endif
}

TThreadLock::~TThreadLock() {
	Destroy();
}

int TThreadLock::Init() {
#ifdef SYSTEM_WIN
	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	_CHECK_RS(m_hMutex, NULL, -1);
#else
	if (0 != pthread_mutex_init(&m_hMutex, NULL))
		return -1;
#endif
	return 0;
}

int TThreadLock::Destroy() {
#ifdef SYSTEM_WIN
	CLOSE_HANDLE(m_hMutex);
#else
	if (0 != pthread_mutex_destroy(&m_hMutex))
		return -1;
#endif
	return 0;
}

int TThreadLock::Lock() {
#ifdef SYSTEM_WIN
	DWORD dw;
	dw = WaitForSingleObject(m_hMutex, INFINITE);
	_CHECK_RS(dw, WAIT_FAILED, -1);
#else
	if (0 != pthread_mutex_lock(&m_hMutex))
		return -1;
#endif
	return 0;
}

int TThreadLock::UnLock() {
#ifdef SYSTEM_WIN
	BOOL b;
	b = ReleaseMutex(m_hMutex);
	_CHECK_RS(b, FALSE, -1);
#else
	if (0 != pthread_mutex_unlock(&m_hMutex))
		return -1;
#endif
	return 0;
}

//-----------------------------------------------------------
//					读写锁
//-----------------------------------------------------------
#ifndef SYSTEM_WIN

/*
 * union semun
 * {
 *  int val;
 *  struct semid_ds *semds;
 *  unsigned short *array;
 *  struct seminfo *info;
 * };
 *     成员说明：
 *     1）val：用于SETVAL命令，设置信号量的值。
 *     2）semds：用于IPC_STAT和IPC_SET命令，指向semid_ds结构，用于获取或设置信号量控制结构。
 *     3）array：用于GETALL和SETALL命令，获取或者设置信号量集的值。
 *     4）info：用于IPC_INFO命令，该命令是Linux系统下特有的，用于返回系统内核定义的信号量极值的定义信息。
 *     该成员为一个结构指针，结构类型struct seminfo的定义如下：
 *     struct  seminfo
 *     {
 *     	int semmap;   //信号量映射入口，暂未用
 *     	int semmni;   //系统中允许创建的信号量集的最大数目
 *      int semmns;   //系统中允许创建的信号量的最大数目
 *      int semmnu;   //暂未用
 *      int semmsl;   //一个信号量集中允许包含的最大信号量数目
 *      int semopm;   //一次semop可以同时操作的最大信号量数目
 *      int semume;   //每个进程允许sem_undo结构的最大数目，暂未用
 *      int semusz;   //sem_undo结构的大小
 *      int semvmx;   //信号量允许设置的最大值
 *      int semaem;   //信号量的sem_undo机制可以记录的最大值
 *     };
 * */

#define MAX_RESOURCE 1024
#define SVSEM_MODE 0644
#define USHORT unsigned short

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array; /* Array for GETALL, SETALL */
	struct seminfo *__buf;
};

TWRLock::TWRLock() {

}

TWRLock::~TWRLock() {

}

int TWRLock::Init(key_t key) {

	int res;
	int flag = IPC_CREAT | IPC_EXCL | SVSEM_MODE;
	/* 创建信号量 */
	if ( ( m_semid = semget( key,2,flag ) ) >= 0 ) {
		sembuf sb[2];	
		memset(sb, 0, sizeof(sb));

		sb[0].sem_num = 0;
		sb[0].sem_op = 0;
		sb[0].sem_flg = 0;

		sb[1].sem_num = 1;
		sb[1].sem_op = MAX_RESOURCE;
		sb[1].sem_flg = 0;

		res = semop(m_semid,sb,2);
		_CHECK_RS(res,-1,-1);
		m_key = key;
	}
	else if ( EEXIST == errno ) {
		/* 连接信号量部分 */
		m_semid = semget( key,2,SVSEM_MODE );
		_CHECK_RS(m_semid,-1,-1);
		// 等待创建进程把信号量创建完成
		int i;
		for ( i = 0; i < MAX_TRIES; i++ ) {
			if ( 0 != SemOtime() ) {
				flag = true;
				break;
			}
			usleep(10);
		}
		if ( MAX_TRIES == i ) {
			_CHECK_RS(-1, -1, -1);
		}
		m_key = key;
	}
	else {
		_CHECK_RS(-1,-1,-1);
	}
	return 0;
}

int TWRLock::Destroy() {
	int res;
	res = semctl(m_semid, 0, IPC_RMID, NULL);
	_CHECK_RS(res, -1, -1);
	return 0;
}

int TWRLock::LockW() {

	int res;
	sembuf sb0[2],sb1;
//	memset(&sb0,0,sizeof(sb0));
	memset(sb0,0,sizeof(sb0));
	memset(&sb1,0,sizeof(sb1));

	sb0[0].sem_num = 0;
	sb0[0].sem_op = 0;
	sb0[0].sem_flg = 0;

	sb0[1].sem_num = 0;
	sb0[1].sem_op = 1;
//	sb0[1].sem_op = -1;
	sb0[1].sem_flg = SEM_UNDO;

	/* 
	 * 这里是先判断pos[0]的信号量是否为0，如果是0的话op为0的操作不会阻塞，
	 * 否则阻塞如果第一个通过了，然后就是锁互斥锁
     * */
redo0:
	res = semop(m_semid,sb0,2);	
	if ( -1 == res && EINTR == errno )
		goto redo0;
	_CHECK_RS(res,-1,-1);

	sb1.sem_num = 1;
	sb1.sem_op = -1 * MAX_RESOURCE;
	sb1.sem_flg = SEM_UNDO;
	/* 占用了互斥锁后，接着就是把所有的资源都占用 */
redo1:
	res = semop(m_semid,&sb1,1);	
	if ( -1 == res && EINTR == errno )
		goto redo1;
	_CHECK_RS(res,-1,-1);

	return 0;
}

int TWRLock::LockR() {

	int res;
	sembuf sb[2];
	/* 这里是先判断pos[0]的信号量是否为0，如果是0的话op为0的操作不会阻塞，否则阻塞
     * 如果第一个通过了，那么会原子性的对资源-1操作
     */
	sb[0].sem_num = 0;
	sb[0].sem_op = 0;
	sb[0].sem_flg = 0;
	sb[1].sem_num = 1;
	sb[1].sem_op = -1;
	sb[1].sem_flg = SEM_UNDO;

redo:
	res = semop(m_semid,sb,2);	
	if ( -1 == res && EINTR == errno ) {
		goto redo;	
	}
	_CHECK_RS(res,-1,-1);	

	return 0;
}

int TWRLock::UnLockW() {

	int res;
	sembuf sb;
	/* 释放资源 */
	sb.sem_num = 1;
	sb.sem_op = MAX_RESOURCE;
	sb.sem_flg = SEM_UNDO;
redo0:
	res = semop(m_semid,&sb,1);
	if ( -1 == res && EINTR == errno ) {
		goto redo0;
	}
	_CHECK_RS(res,-1,-1);

	/* 释放互斥锁 */
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
redo1:
	res = semop(m_semid,&sb,1);
	if ( -1 == res && EINTR == errno ) {
		goto redo1;
	}
	_CHECK_RS(res,-1,-1);	

	return 0;
}

int TWRLock::UnLockR() {

	int res;
	sembuf sb;
	/* 释放资源 */
	sb.sem_num = 1;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;

redo:
	res = semop(m_semid,&sb,1);
	if ( -1 == res && EINTR == errno ) {
		goto redo;
	}
	_CHECK_RS(res,-1,-1);	

	return 0;
}

int TWRLock::Semid() {
	return m_semid;
}

key_t TWRLock::Key() {
	return m_key;
}

time_t TWRLock::SemOtime() {

	time_t t = 0;
	union semun arg;
	struct semid_ds seminfo;
	arg.buf = &seminfo;
	if ( -1 != semctl( m_semid,0,IPC_STAT,arg ) )
		t = arg.buf->sem_otime;
	return t;
}

#endif
