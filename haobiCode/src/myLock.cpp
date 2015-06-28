#include "myDef.h"
#include "myLock.h"
//-----------------------------------------------------------
//					�򵥵��߳���
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
//					��д��
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
 *     ��Ա˵����
 *     1��val������SETVAL��������ź�����ֵ��
 *     2��semds������IPC_STAT��IPC_SET���ָ��semid_ds�ṹ�����ڻ�ȡ�������ź������ƽṹ��
 *     3��array������GETALL��SETALL�����ȡ���������ź�������ֵ��
 *     4��info������IPC_INFO�����������Linuxϵͳ�����еģ����ڷ���ϵͳ�ں˶�����ź�����ֵ�Ķ�����Ϣ��
 *     �ó�ԱΪһ���ṹָ�룬�ṹ����struct seminfo�Ķ������£�
 *     struct  seminfo
 *     {
 *     	int semmap;   //�ź���ӳ����ڣ���δ��
 *     	int semmni;   //ϵͳ�����������ź������������Ŀ
 *      int semmns;   //ϵͳ�����������ź����������Ŀ
 *      int semmnu;   //��δ��
 *      int semmsl;   //һ���ź��������������������ź�����Ŀ
 *      int semopm;   //һ��semop����ͬʱ����������ź�����Ŀ
 *      int semume;   //ÿ����������sem_undo�ṹ�������Ŀ����δ��
 *      int semusz;   //sem_undo�ṹ�Ĵ�С
 *      int semvmx;   //�ź����������õ����ֵ
 *      int semaem;   //�ź�����sem_undo���ƿ��Լ�¼�����ֵ
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
	/* �����ź��� */
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
		/* �����ź������� */
		m_semid = semget( key,2,SVSEM_MODE );
		_CHECK_RS(m_semid,-1,-1);
		// �ȴ��������̰��ź����������
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
	 * ���������ж�pos[0]���ź����Ƿ�Ϊ0�������0�Ļ�opΪ0�Ĳ�������������
	 * �������������һ��ͨ���ˣ�Ȼ�������������
     * */
redo0:
	res = semop(m_semid,sb0,2);	
	if ( -1 == res && EINTR == errno )
		goto redo0;
	_CHECK_RS(res,-1,-1);

	sb1.sem_num = 1;
	sb1.sem_op = -1 * MAX_RESOURCE;
	sb1.sem_flg = SEM_UNDO;
	/* ռ���˻������󣬽��ž��ǰ����е���Դ��ռ�� */
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
	/* ���������ж�pos[0]���ź����Ƿ�Ϊ0�������0�Ļ�opΪ0�Ĳ���������������������
     * �����һ��ͨ���ˣ���ô��ԭ���ԵĶ���Դ-1����
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
	/* �ͷ���Դ */
	sb.sem_num = 1;
	sb.sem_op = MAX_RESOURCE;
	sb.sem_flg = SEM_UNDO;
redo0:
	res = semop(m_semid,&sb,1);
	if ( -1 == res && EINTR == errno ) {
		goto redo0;
	}
	_CHECK_RS(res,-1,-1);

	/* �ͷŻ����� */
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
	/* �ͷ���Դ */
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
