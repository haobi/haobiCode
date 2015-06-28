#include "myThread.h"

//------------------------------------------------------------
//						工作任务类
//------------------------------------------------------------
TThreadTask::TThreadTask() {
	m_bEnd = false;
	m_nResult = TASK_ERROR;
	m_strTaskName = NULL;
}

TThreadTask::~TThreadTask() {
	;
}

int TThreadTask::TaskInit() {
	return 0;
}

int TThreadTask::Run() {
	
	int nRs;

	nRs = TaskInit();
	if (-1 == nRs) {
		SetResult(nRs);
		_CHECK_RS(nRs, -1, -1);
	}

	while (false == GetEnd()) {

		nRs = TaskLoop();
		_CHECK_RS(nRs, TASK_ERROR, -1);

		SetResult(nRs);
		if (TASK_UNFINISH == nRs) {
			continue ;
		}
		else if (TASK_ERROR == nRs || TASK_SUCCEED == nRs || TASK_FAIL == nRs) {
			break ;
		}
	}

	printf("Task[%s] is end, status[%d] .\n", m_strTaskName, nRs);

	return 0;
}
//------------------------------------------------------------
//						工作线程类
//------------------------------------------------------------
// 这个主要是用于切换到工作线程中运行最大需要的消耗时间
// 以便能够让线程的状态标识能够改变
#ifdef SYSTEM_WIN
#define SLEEP_10_MILLS() Sleep(10)
#else
#define SLEEP_10_MILLS() usleep(10000)
#endif

#ifdef SYSTEM_WIN
DWORD WINAPI ThreadProc(LPVOID arg) {

	TThread *pThread = (TThread *)arg;
	pThread->Run();

	return 0;
}
#else
void* ThreadProc(void *arg) {

	TThread *pThread = (TThread *)arg;
	pThread->Run();
}
#endif

TThread::TThread() {

	m_nTicket = 0;
	m_tid = -1;
	m_nStatus = THREAD_UNINITIAL;
	m_bEnd = false;
	m_pTask = NULL;
	m_notify = NULL;
	m_pArg = NULL;

#ifdef SYSTEM_WIN
	m_hEvent = NULL;
	m_hHandle = NULL;
#endif
	m_notify = NULL;
	m_pArg = NULL;
}

TThread::~TThread() {

#ifdef SYSTEM_WIN
	CloseHandle(m_hEvent);
	CloseHandle(m_hHandle);
#else
	pthread_mutex_destroy(&m_Mutex);
	pthread_cond_destroy(&m_Cond);
#endif
}

int TThread::Init() {
	
	int nRs, nTmpTicket;
	
	if (THREAD_UNINITIAL != m_nStatus)
		return 1;

	nTmpTicket = m_nTicket;
#ifdef SYSTEM_WIN
	// 创建事件内核对象
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	_CHECK_RS(m_hEvent, NULL, -1);
	
	m_hHandle = CreateThread(NULL, 0, ThreadProc, this, 0, &m_tid);
	_CHECK_RS(m_hHandle, NULL, -1);
#else
	nRs = pthread_mutex_init(&m_Mutex, NULL);
	_CHECK_RS(nRs, -1, -1);

	nRs = pthread_cond_init(&m_Cond, NULL);
	_CHECK_RS(nRs, -1, -1);

	nRs = pthread_create(&m_tid, NULL, ThreadProc, this);
	_CHECK_RS(nRs, -1, -1);
#endif
	// 休眠一段时间，以保证工作线程已经运作，状态已经更改
	while (nTmpTicket == m_nTicket)
		SLEEP_10_MILLS();
	
	return 0;
}

int TThread::Run() {

	int nResult;
#ifdef SYSTEM_WIN
	BOOL bRs;
	DWORD dw;
#else
	int nRs;
#endif

	m_nStatus = THREAD_IDLE;
	m_nTicket++;
	
	while (false == m_bEnd) {
		// 1、上锁，2、等待条件变量复活
	#ifdef SYSTEM_WIN
		dw = WaitForSingleObject(m_hEvent, INFINITE);
		_CHECK_RS(dw, WAIT_FAILED, -1);
	#else
		nRs = pthread_mutex_lock(&m_Mutex);	
		_CHECK_RS(nRs, -1, -1);

		nRs = pthread_cond_wait(&m_Cond, &m_Mutex);
		_CHECK_RS(nRs, -1, -1);
	#endif
		printf("thread get signal .\n");
//		m_nTicket++;
		if (NULL != m_pTask) {

			m_nStatus = THREAD_BUSY;
			m_nTicket++;
			m_pTask->Run();
			printf("thread_task[%s] after run .\n", m_pTask->GetTaskName());
			// 判断是否有设置回调函数，设置了则进行调用
			if (NULL != m_notify) {

				m_notify(m_pArg);
				m_notify = NULL;
				m_pArg = NULL;
			}
			
			m_pTask = NULL;
		}

	#ifdef SYSTEM_WIN
		bRs = ResetEvent(m_hEvent);
		_CHECK_RS(bRs, FALSE, -1);
	#else
		nRs = pthread_mutex_unlock(&m_Mutex);	
		_CHECK_RS(nRs, -1, -1);
	#endif

		m_nStatus = THREAD_IDLE;
		printf("thread status[%d] .\n", m_nStatus);
	}
	
	m_nStatus = THREAD_TERMINAL;

	return 0;
};

int TThread::WaitThreadEnd() {

#ifdef SYSTEM_WIN
	BOOL bRs;
#else
	int nRs;
#endif

	if (THREAD_UNINITIAL == m_nStatus)
		return 1;
	if (THREAD_TERMINAL == m_nStatus)
		return 0;
#ifdef SYSTEM_WIN
	bRs = WaitForSingleObject(m_hHandle, INFINITE);
	_CHECK_RS(bRs, WAIT_FAILED, -1);
#else
	nRs = pthread_join(m_tid, NULL);
	_CHECK_RS(nRs, -1, -1);
#endif

	return 0;
}

int TThread::TerminalThread(bool bForce) {

#ifdef SYSTEM_WIN
	BOOL bRs;
#else
	int nRs;
#endif

	if (true == bForce) {
	#ifdef SYSTEM_WIN
		bRs = TerminateThread(m_hHandle, 0);
		_CHECK_RS(bRs, FALSE, -1);
	#else
		nRs = pthread_cancel(m_tid);
		_CHECK_RS(nRs, -1, -1);
	#endif
		m_nStatus = THREAD_TERMINAL;
	}
	else {
		TerminalTask();
	}
	
	m_bEnd = true;
	if (THREAD_IDLE == m_nStatus) {

	#ifdef SYSTEM_WIN
		bRs = SetEvent(m_hEvent);
		_CHECK_RS(bRs, FALSE, -1);
	#else
		nRs = pthread_cond_signal(&m_Cond);
		_CHECK_RS(nRs, -1, -1);
	#endif
	}

	return 0;
}

int TThread::StartTask(TThreadTask *pTask, ThreadNotify notify, void *arg) {

	int nTmpTicket;
#ifdef SYSTEM_WIN
	BOOL bRs;
	DWORD dw;
#else
	int nRs;
#endif

	_CHECK_RS(pTask, NULL, -1);

	printf("StartTask | m_nStatus[%d] .\n", m_nStatus);
	if (THREAD_IDLE != m_nStatus)
		return 1;

	nTmpTicket = m_nTicket;

	m_pTask = pTask;
	m_notify = notify;
	m_pArg = arg;

	printf("set thread signal .\n");
#ifdef SYSTEM_WIN
	bRs = SetEvent(m_hEvent);
	_CHECK_RS(bRs, FALSE, -1);
#else
	nRs = pthread_cond_signal(&m_Cond);
	_CHECK_RS(nRs, -1, -1);
#endif
	// 休眠一段时间，以保证工作线程已经运作，状态已经更改
	while (nTmpTicket == m_nTicket)
		SLEEP_10_MILLS();
	printf("start_task is running .\n");

	return 0;
}

void TThread::TerminalTask(bool bWait) {

	if (NULL == m_pTask)
		return ;
	
	m_pTask->SetEnd();
	if (false == bWait)
		return ;
		
	while (THREAD_BUSY == m_nStatus)
		SLEEP_10_MILLS();
}
