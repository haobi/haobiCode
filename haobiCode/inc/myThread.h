#ifndef _MY_THREAD_H
#define _MY_THREAD_H

#include "myDef.h"

#ifndef SYSTEM_WIN
#include <pthread.h>
#endif
//------------------------------------------------------------
//						工作任务类
//------------------------------------------------------------
class TThreadTask {

public:
	TThreadTask();
	virtual ~TThreadTask();

	// 返回值，TASK_SUCCEED：处理成功，TASK_FAIL：处理失败
	int Run();
	virtual int TaskInit();
	virtual int TaskLoop() = 0;
	
	inline int GetResult() {return m_nResult;}

	void SetEnd() {m_bEnd = true;}
	inline bool GetEnd() {return m_bEnd;}

	inline const char* GetTaskName() {return m_strTaskName;}

	enum  {
		TASK_ERROR = -1,	// 处理任务时出现错误
		TASK_SUCCEED,		// 任务处理成功
		TASK_FAIL,			// 任务处理失败
		TASK_UNFINISH,	// 任务继续处理，即继续调用TaskLoop()函数
	};

private:
	inline void SetResult(int nResult) {m_nResult = nResult;}

protected:
	const char *m_strTaskName;

private:
	int m_nResult;
	bool m_bEnd;
};
//------------------------------------------------------------
//						工作线程类
//------------------------------------------------------------
#ifdef SYSTEM_WIN
DWORD WINAPI ThreadProc(LPVOID arg);
#else
void* ThreadProc(void *arg);	
#endif
typedef void* (*ThreadNotify)(void *arg);
/*
线程类不考虑多线程互斥操作的情况
*/
class TThread {

public:
	TThread();
	~TThread();

	int Init();
	int Run();
	int WaitThreadEnd();
	// bForce表示是否强制立刻结束线程，true为立刻结束，false等待当前任务处理完成后结束
	// 不建议使用bForce = true的方式，因为有可能会出现线程强制退出后，死锁的状况
	int TerminalThread(bool bForce = false);
	// bWait表示是否等待，直到任务结束为止
	void TerminalTask(bool bWait = true);
	// 设置线程处理任务，设置后线程立刻对任务进行处理
	// 设置线程处理完后，回调后续的通知操作，若没有后续的操作，可以不设置
	int StartTask(TThreadTask *pTask, ThreadNotify notify = NULL, void *arg = NULL);
	
	inline TID GetThreadID() {return m_tid;}
	inline int GetStatus() {return m_nStatus;}
	
	enum THREAD_STATUS {
		THREAD_UNINITIAL = -1,
		THREAD_IDLE,
		THREAD_BUSY,
		THREAD_TERMINAL,
	};
private:
	unsigned int m_nTicket;		// 辅助状态变化的变量
	TID m_tid;		// 线程ID
	int m_nStatus;	// 线程当前状态，参照THREAD_STATUS
	bool m_bEnd;	// 是否终结线程的标识

	TThreadTask *m_pTask;

#ifdef SYSTEM_WIN
	HANDLE m_hEvent;	// 等待事件
	HANDLE m_hHandle;	// 线程句柄
#else
	pthread_mutex_t m_Mutex;	// 互斥锁 + 条件变量 做成事件等待模式
	pthread_cond_t m_Cond;
#endif

	ThreadNotify m_notify;	// 处理完成后的，通知回调函数
	void *m_pArg;		// 回调函数的参数
};

/*
m_nStatus的状态变换全部都在线程中修改，主线程不得进行改变
*/

#endif
