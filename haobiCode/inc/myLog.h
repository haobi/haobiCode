#ifndef _MY_LOG_H
#define _MY_LOG_H

#include "myDef.h"
#include "myLock.h"

// 默认每个日志文件的最大容量，超过则切换日志
const int LOG_DEF_MAX_SIZE = 1024 * 1024 * 500;
// 日志名格式为：[程序名称]_[YYYYMMDD][.日志序号].log，当前日志木有[.日志序号]一列
// 当前日志,1,2,...,99
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
		DATE_CHANGE = 1,	// 因为日期切换
		SIZE_CHANGE,		// 因为文件大小切换
	};

	TLog();
	~TLog();
	/*, bool threadMode = true*/
	int Init(const char *dir, int size = -1, bool debug = false);
	void Close();

	int Log(LOG_MODE level, const char *file, int line, const char *buf, ...);
private:
	int LogR(const char *buf);
	// 返回值：
    // 0：不需要启用新日志，1：需要启用新日志，-1：出错
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
	// __VA_ARGS__要求在VS2002(VC7.0)以上的版本
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
