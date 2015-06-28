#ifndef _MY_LOCK_H
#define _MY_LOCK_H

#include "myDef.h"

#ifdef SYSTEM_WIN
	#include <windows.h>
#else
	#include <pthread.h>
	#include <sys/wait.h>
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
#endif

//-----------------------------------------------------------
//					�򵥵��߳���
//-----------------------------------------------------------
class TThreadLock {

public:
	TThreadLock();
	~TThreadLock();

	int Init();
	int Destroy();
	int Lock();
	int UnLock();

private:
#ifdef SYSTEM_WIN
	HANDLE m_hMutex;
#else
	pthread_mutex_t m_hMutex;
#endif
};
//-----------------------------------------------------------
//					��д��
//-----------------------------------------------------------
#ifndef SYSTEM_WIN

#include <myDef.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/* ���ֻ����֧��ͬʱMAX_RESOURCE������ͬʱ�� */
#define MAX_RESOURCE	1024

/*
 * һ��Ķ�д����
 * һ��Ķ�д������һ��ʼ��������MAX_RESOURCE����Դ������д������ʱ���һ����ռ��
 * MAX_RESOURCE����Դ������������ʱ���ֻ��ռ��һ����Դ�������ӻ����һ���������
 * ����ڵ�ǰ��Դ����Ŀ��ΪMAX_RESOURCE��ʱ����ô���ǲ��ܽ���д������ֻ���ǽ���
 * �������������ͣ���ж������Ļ�����ô���޷�����д��������ô���ǳ����׳Ƶ�"д����"
 * ״̬����Ҳ�Ƕ����������������ڡ�
 *
 * д���ȶ�д����
 * ��˵��һ�£�д���ȶ�д�������������Ĵ��ڣ�������(A)����Դ��(B)
 * �ڽ���ռ����Դ��ʱ��
 *
 * ������:
 * �����ж�A�Ƿ������ˣ��ǵĻ��ȴ�ֱ������Ϊֹ����û���������B���м���(��һ����Դ��)������
 *
 * д������
 * ���ж�A�Ƿ��������ǵĻ��ȴ�ֱ������Ϊֹ����û�����A���м���������Ȼ���B���м���
 * ����(��MAX_RESOURCE����Դ)����
 *
 * ����ʵ��ԭ��
 * ��һ��ֵ��ȥע��ĵط���
 * ������ô�ܹ�ʵ���жϻ������Ƿ�ռ�ã��������ǲ������������ģ�ֻ��Ҫ�жϻ������Ƿ�
 * ����ס�ˡ���ô�Ͳ�����һ���semop������Դ�����ˡ�������Ļ�����ȡ�ɵ�ʹ��semop(0)��
 * ��ָ�����ź���ռ��0����Դ�Ĳ���(���ռ��0����Դ���������Ҫ�ȴ�����Դ��Ϊ0ʱ�ŷ���)��
 * �������ǾͿ�������һ��ԭ�Ӳ�����
 * 1��semop[0] : op = 0;
 * 2��semop[1] : op = -1;
 * ������������������һ��������Ϳ���ʵ��ԭ�ӵ����жϣ��ٲ�����Ч����
 *
 * */

//=================================================================
/* д���ȶ�д�� */
class TWRLock {

public:
	TWRLock();
	~TWRLock();

	int Init(key_t key);
	int Destroy();

	int LockW();
	int LockR();

	int UnLockW();
	int UnLockR();

	time_t SemOtime();
	int Semid();
	key_t Key();

private:
	int m_semid;
	key_t m_key;
};

#endif

#endif