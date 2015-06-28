#ifndef _MY_THREAD_H
#define _MY_THREAD_H

#include "myDef.h"

#ifndef SYSTEM_WIN
#include <pthread.h>
#endif
//------------------------------------------------------------
//						����������
//------------------------------------------------------------
class TThreadTask {

public:
	TThreadTask();
	virtual ~TThreadTask();

	// ����ֵ��TASK_SUCCEED������ɹ���TASK_FAIL������ʧ��
	int Run();
	virtual int TaskInit();
	virtual int TaskLoop() = 0;
	
	inline int GetResult() {return m_nResult;}

	void SetEnd() {m_bEnd = true;}
	inline bool GetEnd() {return m_bEnd;}

	inline const char* GetTaskName() {return m_strTaskName;}

	enum  {
		TASK_ERROR = -1,	// ��������ʱ���ִ���
		TASK_SUCCEED,		// ������ɹ�
		TASK_FAIL,			// ������ʧ��
		TASK_UNFINISH,	// ���������������������TaskLoop()����
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
//						�����߳���
//------------------------------------------------------------
#ifdef SYSTEM_WIN
DWORD WINAPI ThreadProc(LPVOID arg);
#else
void* ThreadProc(void *arg);	
#endif
typedef void* (*ThreadNotify)(void *arg);
/*
�߳��಻���Ƕ��̻߳�����������
*/
class TThread {

public:
	TThread();
	~TThread();

	int Init();
	int Run();
	int WaitThreadEnd();
	// bForce��ʾ�Ƿ�ǿ�����̽����̣߳�trueΪ���̽�����false�ȴ���ǰ��������ɺ����
	// ������ʹ��bForce = true�ķ�ʽ����Ϊ�п��ܻ�����߳�ǿ���˳���������״��
	int TerminalThread(bool bForce = false);
	// bWait��ʾ�Ƿ�ȴ���ֱ���������Ϊֹ
	void TerminalTask(bool bWait = true);
	// �����̴߳����������ú��߳����̶�������д���
	// �����̴߳�����󣬻ص�������֪ͨ��������û�к����Ĳ��������Բ�����
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
	unsigned int m_nTicket;		// ����״̬�仯�ı���
	TID m_tid;		// �߳�ID
	int m_nStatus;	// �̵߳�ǰ״̬������THREAD_STATUS
	bool m_bEnd;	// �Ƿ��ս��̵߳ı�ʶ

	TThreadTask *m_pTask;

#ifdef SYSTEM_WIN
	HANDLE m_hEvent;	// �ȴ��¼�
	HANDLE m_hHandle;	// �߳̾��
#else
	pthread_mutex_t m_Mutex;	// ������ + �������� �����¼��ȴ�ģʽ
	pthread_cond_t m_Cond;
#endif

	ThreadNotify m_notify;	// ������ɺ�ģ�֪ͨ�ص�����
	void *m_pArg;		// �ص������Ĳ���
};

/*
m_nStatus��״̬�任ȫ�������߳����޸ģ����̲߳��ý��иı�
*/

#endif
