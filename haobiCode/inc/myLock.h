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
//					简单的线程锁
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
//					读写锁
//-----------------------------------------------------------
#ifndef SYSTEM_WIN

#include <myDef.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/* 最大只可以支持同时MAX_RESOURCE个进程同时读 */
#define MAX_RESOURCE	1024

/*
 * 一般的读写锁：
 * 一般的读写锁都是一开始对锁分配MAX_RESOURCE个资源，其中写操作的时候会一次性占用
 * MAX_RESOURCE个资源，而读操作的时候就只会占用一个资源。这样子会出现一个问题就是
 * 如果在当前资源的数目不为MAX_RESOURCE的时候，那么总是不能进行写操作，只能是进行
 * 读操作，如果不停的有读操作的话，那么就无法进行写操作，那么就是出现俗称的"写饿死"
 * 状态，这也是读优先锁的问题所在。
 *
 * 写优先读写锁：
 * 先说明一下，写优先读写锁会有两把锁的存在：互斥锁(A)、资源锁(B)
 * 在进行占用资源的时候：
 *
 * 读操作:
 * 会先判断A是否上锁了，是的话等待直到解锁为止，若没有上锁则对B进行加锁(锁一个资源给)操作。
 *
 * 写操作：
 * 会判断A是否上锁，是的话等待直到解锁为止，若没有则对A进行加锁操作，然后对B进行加锁
 * 操作(锁MAX_RESOURCE个资源)操作
 *
 * 具体实现原理：
 * 有一个值得去注意的地方：
 * 就是怎么能够实现判断互斥锁是否被占用，读操作是不用锁互斥锁的，只是要判断互斥锁是否
 * 被锁住了。那么就不能用一般的semop增减资源操作了。在这里的话可以取巧的使用semop(0)即
 * 对指定的信号量占用0个资源的操作(如果占用0个资源，这个操作要等待到资源给为0时才返回)。
 * 这样我们就可以运用一对原子操作：
 * 1、semop[0] : op = 0;
 * 2、semop[1] : op = -1;
 * 将这两个操作都放在一个数组里，就可以实现原子的先判断，再操作的效果了
 *
 * */

//=================================================================
/* 写优先读写锁 */
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