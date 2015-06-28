#ifndef _MY_DEF_H
#define _MY_DEF_H

#include <stdio.h>
#include <stdlib.h>

#ifdef SYSTEM_WIN
	#include <windows.h>
	#include <direct.h>
	#include <io.h>
#else
	#include <unistd.h>
	#include <sys/select.h>
	#include <sys/time.h>
	
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <signal.h>

	#include <sys/mman.h>
	#include <sys/ipc.h>
	#include <sys/shm.h>
	#include <sys/sem.h>
	#include <sys/msg.h>
#endif
	#include <sys/types.h>
	#include <sys/stat.h>

	#include <errno.h>
	#include <string.h>
	#include <fcntl.h>
	#include <time.h>

// 函数方面的兼容
#ifdef SYSTEM_WIN
	#define snprintf _snprintf

#else
	#define GetCurrentProcessId getpid
	#define GetCurrentThreadId pthread_self
#endif

// 类型定义方面的兼容
#ifdef SYSTEM_WIN
	#define TID DWORD		// unsigned long
	#define PID DWORD
#else
	#define TID pthread_t	// pthread_t --> unsigned long int
	#define PID pid_t		// pid --> int
	#define LOCK_FILE "/var/run"
#endif

#define PVOID void*

// 此类型是为了防止在32位的机器上获取2GB以上大小的文件，导致获取文件大小不准确的问题
#define size_f long long
// 判断是否为32位，否则64位
#define BIT_32 ((sizeof(long) == 4 ? true : false))
const long long SIZE_1G = (1024 * 1024 * 1024);
const long long SIZE_4G = 4 * SIZE_1G;

#ifndef SYSTEM_WIN
	typedef void Sigfunc(int);
#endif

#ifdef SYSTEM_WIN
/*
#define _CHECK_RS(rs, mark, res) \
	do { \
		if (rs == mark) { \
			printf("[FILE:%s,LINE:%d] error[%d] : %s\n", __FILE__, __LINE__, GetLastError(), strerror(errno)); \
			return res; \
		} \
	} while (0)
#else
*/
#define _CHECK_RS(rs, mark, res) \
	do { \
		if (rs == mark) { \
			printf("[FILE:%s,LINE:%d] error[%d]\n", __FILE__, __LINE__, GetLastError()); \
			return res; \
		} \
	} while (0)
#else
#define _CHECK_RS(rs, mark, res) \
	do { \
		if (rs == mark) { \
			printf("[FILE:%s,LINE:%d] error[%d] : %s\n", __FILE__, __LINE__, errno, strerror(errno)); \
			return res; \
		} \
	} while (0)
#endif

#ifdef SYSTEM_WIN
#define CLOSE_HANDLE(h) \
	do { \
		if (h) { \
			CloseHandle(h); \
			h = NULL; \
		} \
	} while (0)
#endif

#define CLOSE_F(f) \
	do { \
		if (-1 != f) { \
			close(f); \
			f = -1; \
		} \
	} while (0)

#define CLOSE_FP(fp) \
	do { \
		if (fp) { \
			fclose(fp); \
			fp = NULL; \
		} \
	} while (0)

#define _CHECK_GOTO(rs, mark, symbol) \
do { \
	if (rs == mark) { \
		goto symbol; \
	} \
} while (0)
/*
#define _CHECK_BREAK(rs, mark) \
	if (rs == mark) { \
		break ; \
	}
*/
#define FREE_PT(p) \
	do { \
		if (p) { \
			free(p); \
			p = NULL; \
		} \
	} while (0)

#define RELEASE_PT(p) \
	do { \
		if (p) { \
			delete p; \
			p = NULL; \
		} \
	} while (0)

#define RELEASE_SZ(p) \
	do { \
		if (p) { \
			delete[] p; \
			p = NULL; \
		} \
	} while (0)

// 简单的只能指针，主要负责释放内存
class TAutoPtr {
public:
	TAutoPtr() {
		m_pt = NULL;
	};

	~TAutoPtr() {
		FREE_PT(m_pt);
	}

	void *m_pt;
};
//----------------------------------------------------------
#ifndef SYSTEM_WIN
	Sigfunc * hb_signal(int signo,Sigfunc *func);
#endif

bool OverNGB(size_f size, int nGB);

bool IsBigEndian();

const char* time2str(time_t t, char *buf);

const char* ThreadId2Str(TID tid, char *buf);

const char* ProcessId2Str(PID pid, char *buf);

#endif
