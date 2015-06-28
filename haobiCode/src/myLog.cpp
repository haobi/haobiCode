#include <stdarg.h>
#include "myLog.h"
#include "myfdcntl.h"

const char* strLogMode[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

const char* GetDate(char *buf) {

	time_t t;
	struct tm tt;

	_CHECK_RS(buf, NULL, NULL);

	t = time(NULL);
#ifdef SYSTEM_WIN
	localtime_s(&tt, &t);
#else
	localtime_r(&t, &tt);	
#endif

	sprintf(buf, "%04d%02d%02d", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday);
	return buf;
}

#ifdef SYSTEM_WIN
const char* GetTime(char *buf) {

	SYSTEMTIME s;
	
	memset(&s, 0, sizeof(s));

	GetSystemTime(&s);
	GetLocalTime(&s);
	
	sprintf(buf, "%02d:%02d:%02d.%03d", 
		s.wHour, s.wMinute, s.wSecond, s.wMilliseconds);

	return buf;
}
#else
const char* GetTime(char *buf) {

	int nRs;
	struct tm tt;
	struct timeval tv;

	_CHECK_RS(buf, NULL, NULL);

	nRs = gettimeofday(&tv, NULL);
	if (0 != nRs)
		return NULL;

	localtime_r(&tv.tv_sec, &tt);

	sprintf(buf, "%02d:%02d:%02d.%06ld", 
		tt.tm_hour, tt.tm_min, tt.tm_sec, tv.tv_usec);

	return buf;
}
#endif
//---------------------------------------------------------------
TLog::TLog() {

	m_fd = -1;
	m_nSize = -1;
	
	m_bDebug = false;

	memset(m_strModuleName, 0, sizeof(m_strModuleName));
	memset(m_strDate, 0, sizeof(m_strDate));
	memset(m_strDir, 0, sizeof(m_strDir));

	m_pLock = NULL;
}

TLog::~TLog() {
	Close();
	RELEASE_PT(m_pLock);
}

int TLog::Init(const char *dir, int size, bool debug) {

	int nRs;

	_CHECK_RS(dir, NULL, -1);

	m_pLock = new TThreadLock;
	_CHECK_RS(m_pLock, NULL, -1);

	if (0 > size)
		m_nSize = LOG_DEF_MAX_SIZE;
	else
		m_nSize = size;

	m_bDebug = debug;
	strncpy(m_strDir, dir, sizeof(m_strDir));

	GetExeName(m_strModuleName);

	nRs = m_pLock->Init();
	_CHECK_RS(nRs, -1, -1);

	nRs = CreateLog();
	_CHECK_RS(nRs, -1, -1);

	SetTime(m_tv);

	return 0;
}

void TLog::Close() {
	CLOSE_F(m_fd);
	m_pLock->Destroy();
}

int TLog::Log(LOG_MODE level, const char *file, int line, const char *buf, ...) {

	char buf0[512] = {0};
	char buf1[512] = {0};
	char strTime[32] = {0};
	const char *pFile;
	va_list ap;

	_CHECK_RS(buf, NULL, -1);

	if (false == m_bDebug && DEBUG_MODE == level)
		return 0;

	va_start(ap, buf);
    vsprintf(buf0, buf, ap);
    va_end(ap);

	pFile = file;

	snprintf(buf1, sizeof(buf1), "[%s,%s,%d,%lu,%s] %s\n", 
		GetTime(strTime), pFile, line, GetCurrentThreadId(), strLogMode[level], buf0);	

	return LogR(buf1);
}

#ifdef SYSTEM_WIN
int TLog::LogR(const char *buf) {

	int nLen, nRs;
	bool b;

	_CHECK_RS(buf, NULL, -1);

	b = false;
	nLen = strlen(buf);

	nRs = m_pLock->Lock();
	_CHECK_RS(nRs, -1, -1);

	SetTime(m_curTV);

	if (true == CheckChangeInter()) {
	
		nRs = NeedChangeLog();
		_CHECK_GOTO(nRs, -1, unlock_out);

		if (0 != nRs) {
		
			CLOSE_F(m_fd);
			if (SIZE_CHANGE == nRs) {
				nRs = RenameHistoryLog();
				_CHECK_GOTO(nRs, -1, unlock_out);
			}

			nRs = CreateLog();
			_CHECK_GOTO(nRs, -1, unlock_out);
		}
		
		SetTime(m_tv);
	}

	nRs = write(m_fd, buf, nLen);
	_CHECK_GOTO(nRs, -1, unlock_out);

	b = true;
unlock_out:
	nRs = m_pLock->UnLock();
	_CHECK_RS(nRs, -1, -1);

	if (false == b)
		return -1;
	return 0;
}
#else
int TLog::LogR(const char *buf) {

	int nLen, nRs;

	_CHECK_RS(buf, NULL, -1);

	SetTime(m_curTV);

	if (true == CheckChangeInter()) {
		
		bool b = false;

		nRs = m_pLock->Lock();
		_CHECK_RS(nRs, -1, -1);

		if (true == CheckChangeInter()) {

			nRs = NeedChangeLog();
			_CHECK_GOTO(nRs, -1, unlock_out);

			if (0 != nRs) {
				if (SIZE_CHANGE == nRs) {
					nRs = RenameHistoryLog();
					_CHECK_GOTO(nRs, -1, unlock_out);
				}

				nRs = CreateLog();
				_CHECK_GOTO(nRs, -1, unlock_out);
			}
		}
		b = true;
unlock_out:
		SetTime(m_tv);

		nRs = m_pLock->UnLock();
		_CHECK_RS(nRs, -1, -1);

		if (false == b)
			return -1;
	}

	nLen = strlen(buf);
	nRs = write(m_fd, buf, nLen);
	_CHECK_RS(nRs, -1, -1);

	return 0;
}
#endif
int TLog::NeedChangeLog() {

	size_f nSize;
	char strDate[32] = {0};

	// 1.满足日志容量超过最大阀值
	nSize = GetFileSize(m_fd);
	if (LOG_DEF_MAX_SIZE <= nSize)
		return SIZE_CHANGE;

	GetDate(strDate);
	// 2.判断日期是否已经变迁
	if (0 != strcmp(m_strDate, strDate))
		return DATE_CHANGE;

	return 0;
}

int TLog::CreateLog() {

	int nRs;
	int nTmpFd = -1;
	char strDir[512] = {0};
	char strPath[512] = {0};

	GetDate(m_strDate);

	snprintf(strDir, sizeof(strDir), "%s/%s", m_strDir, m_strDate);
	snprintf(strPath, sizeof(strPath), "%s/%s_%s.log", strDir, m_strModuleName, m_strDate);
	
	nRs = DirExist(strDir);
	_CHECK_RS(nRs, -1, -1);

	if (1 == nRs) {
		nRs = MkDir(strDir);
		_CHECK_RS(nRs, -1, -1);
	}

	// 日志轮替不锁操作
	if (-1 != m_fd)
		nTmpFd = m_fd;

	m_fd = open(strPath, O_WRONLY | O_APPEND | O_CREAT, FILE_MODE);
	_CHECK_RS(m_fd, -1, -1);

	CLOSE_F(nTmpFd);

	return 0;
}

int TLog::RenameHistoryLog() {

	int nRs, nFileNo;	
	char strPath[512];
	char strSrcFile[512];
	char strDstFile[512];

	nFileNo = LOG_MAX_COUNT - 2;
	snprintf(strPath, sizeof(strPath), "%s/%s", m_strDir, m_strDate);

	// debug模式先删掉最后一个文件，如果为日用的话建议将rename的返回值判断忽略
	snprintf(strDstFile, sizeof(strDstFile), "%s/%s_%s.log.%d", 
		strPath, m_strModuleName, m_strDate, nFileNo + 1);
	unlink(strDstFile);

	// 不论是否存在该文件，照样按照有的情况搬迁
	// 日志文件依次为：module_xxx.log, module_xxx.log.1, ..., module_xxx.log.99
	for (int i = nFileNo; i > 0; i--) {
		snprintf(strSrcFile, sizeof(strSrcFile), "%s/%s_%s.log.%d", 
			strPath, m_strModuleName, m_strDate, i);
		snprintf(strDstFile, sizeof(strDstFile), "%s/%s_%s.log.%d", 
			strPath, m_strModuleName, m_strDate, i + 1);
		nRs = FileExist(strSrcFile);
		_CHECK_RS(nRs, -1, -1);
		if (0 == nRs) {
			nRs = rename(strSrcFile, strDstFile);
			_CHECK_RS(nRs, -1, -1);
		}
	}

	// 当前日志进行搬迁
	snprintf(strSrcFile, sizeof(strSrcFile), "%s/%s_%s.log", strPath, m_strModuleName, m_strDate);
	snprintf(strDstFile, sizeof(strDstFile), "%s/%s_%s.log.1", strPath, m_strModuleName, m_strDate);
	nRs = rename(strSrcFile, strDstFile);	
	_CHECK_RS(nRs, -1, -1);

	return 0;
}

#ifdef SYSTEM_WIN
void TLog::SetTime(DWORD &tv) {
	tv = GetTickCount();
}
#else
void TLog::SetTime(struct timeval &tv) {
	gettimeofday(&tv, NULL);
}
#endif

bool TLog::CheckChangeInter() {

#ifdef SYSTEM_WIN
	if (INTERVAL_TIME < m_curTV - m_tv)
		return true;
#else
	long sec, usec, msec;
	
	sec = m_curTV.tv_sec - m_tv.tv_sec;
	usec = m_curTV.tv_usec - m_tv.tv_usec;

	if (0 > usec) {
		sec--;
		usec += 1000000;
	}

	usec = sec * 1000000 + usec;
	msec = usec / 1000;
	if (INTERVAL_TIME < msec)
		return true;
#endif

	return false;
}
