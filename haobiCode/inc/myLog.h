#ifndef _MY_LOG_H
#define _MY_LOG_H

#include "myDef.h"
#include "myLock.h"

// Ĭ��ÿ����־�ļ�������������������л���־
const int LOG_DEF_MAX_SIZE = 1024 * 1024 * 500;
// ��־����ʽΪ��[��������]_[YYYYMMDD][.��־���].log����ǰ��־ľ��[.��־���]һ��
// ��ǰ��־,1,2,...,99
const int LOG_MAX_COUNT = 100;

const long INTERVAL_TIME = 1;

#ifdef SYSTEM_WIN
#define SLEEP_MOMENT() Sleep(1);
#else
#define SLEEP_MOMENT() usleep(1);
#endif
//----------------------------------------------------------
class TLog {

public:
	enum LOG_MODE {
		DEBUG_MODE = 0,
		INFO_MODE,
		WARN_MODE,
		ERROR_MODE,
		FATAL_MODE,
	};

	enum CHANGE_REASON{
		DATE_CHANGE = 1,	// ��Ϊ�����л�
		SIZE_CHANGE,		// ��Ϊ�ļ���С�л�
	};

	TLog();
	~TLog();
	/*, bool threadMode = true*/
	int Init(const char *dir, int size = -1, bool debug = false);
	void Close();

	int Log(LOG_MODE level, const char *file, int line, const char *buf, ...);
private:
	int LogR(const char *buf);
	// ����ֵ��
    // 0������Ҫ��������־��1����Ҫ��������־��-1������
	int NeedChangeLog();
	int CreateLog();
	int RenameHistoryLog();
#ifdef SYSTEM_WIN
	void SetTime(DWORD &tv);
#else
	void SetTime(struct timeval &tv);
#endif
	bool CheckChangeInter();

	char m_strModuleName[128];
	char m_strDate[32];
	char m_strDir[512];

	int m_fd;
	int m_nSize;
	bool m_bDebug;

#ifdef SYSTEM_WIN
	DWORD m_tv;
	DWORD m_curTV;
#else
	struct timeval m_tv;
	struct timeval m_curTV;
#endif

	TThreadLock *m_pLock;
};


#ifdef SYSTEM_WIN
	// __VA_ARGS__Ҫ����VS2002(VC7.0)���ϵİ汾
	#define Debug(buf, ...) Log(TLog::DEBUG_MODE, __FILE__, __LINE__, buf, __VA_ARGS__)
	#define Info(buf, ...)  Log(TLog::INFO_MODE,  __FILE__, __LINE__, buf, __VA_ARGS__)
	#define Warn(buf, ...)  Log(TLog::WARN_MODE,  __FILE__, __LINE__, buf, __VA_ARGS__)
	#define Error(buf, ...) Log(TLog::ERROR_MODE, __FILE__, __LINE__, buf, __VA_ARGS__)
	#define Fatal(buf, ...) Log(TLog::FATAL_MODE, __FILE__, __LINE__, buf, __VA_ARGS__)
#else
	#define Debug(buf, arg...) Log(TLog::DEBUG_MODE, __FILE__, __LINE__, buf, ##arg)
	#define Info(buf, arg...)  Log(TLog::INFO_MODE,  __FILE__, __LINE__, buf, ##arg)
	#define Warn(buf, arg...)  Log(TLog::WARN_MODE,  __FILE__, __LINE__, buf, ##arg)
	#define Error(buf, arg...) Log(TLog::ERROR_MODE, __FILE__, __LINE__, buf, ##arg)
	#define Fatal(buf, arg...) Log(TLog::FATAL_MODE, __FILE__, __LINE__, buf, ##arg)
#endif

#endif
