#ifndef _MEM_REG_H
#define _MEM_REG_H

#include "myDef.h"
#include "myLock.h"

#include <map>

using namespace std;

// #define _MEM_REG_DEBUG

#ifdef _MEM_REG_DEBUG
#define MM_DEFINE() \
        TMemReg *g_mr = NULL;
#else
#define MM_DEFINE()
#endif

#ifdef _MEM_REG_DEBUG
#define MM_INIT() \
    do { \
        if (!g_mr) { \
            g_mr = new TMemReg; \
			g_mr->Init(); \
        } \
    } while (0)
#else
#define MM_INIT()
#endif

#ifdef _MEM_REG_DEBUG
#define MM_DESTROY() \
    do { \
        if (g_mr) { \
            RELEASE_PT(g_mr); \
        } \
    } while (0)
#else
#define MM_DESTROY()
#endif

#ifdef _MEM_REG_DEBUG
#define MM_ALLOC(size) \
	g_mr->Add(calloc(size, 1), size, __LINE__, __FILE__);
#else
#define MM_ALLOC(size) \
	calloc(size, 1);
#endif

#ifdef _MEM_REG_DEBUG
#define MM_FREE(pt) \
    do { \
        g_mr->Del(pt); \
        FREE_PT(pt); \
    } while (0)
#else
#define MM_FREE(pt) \
	do { \
	FREE_PT(pt); \
	} while (0)
#endif

#ifdef _MEM_REG_DEBUG
#define MM_NEW(CLASS) \
	(CLASS *)(g_mr->Add(new CLASS, sizeof(CLASS), __LINE__, __FILE__));
#else
#define MM_NEW(CLASS) \
	new CLASS;
#endif

#ifdef _MEM_REG_DEBUG
#define MM_NEW_ARRAY(CLASS, cnt) \
	(CLASS *)(g_mr->Add(new CLASS[cnt], cnt * sizeof(CLASS), __LINE__, __FILE__));
#else
#define MM_NEW_ARRAY(CLASS, cnt) \
	new CLASS[cnt];
#endif

#ifdef _MEM_REG_DEBUG
#define MM_DELETE(CLASS, pt) \
	do { \
		g_mr->Del(pt); \
		delete (CLASS *)pt; \
		pt = NULL; \
	} while (0)
#else
#define MM_DELETE(CLASS, pt) \
	do { \
		delete (CLASS *)pt; \
		pt = NULL; \
	} while (0)
#endif

#ifdef _MEM_REG_DEBUG
#define MM_DELETE_ARRAY(CLASS, pt) \
	do { \
		g_mr->Del(pt); \
		delete[] (CLASS *)pt; \
		pt = NULL; \
	} while (0)
#else
#define MM_DELETE_ARRAY(CLASS, pt) \
	do { \
		delete[] (CLASS *)pt; \
		pt = NULL; \
	} while (0)
#endif

#ifdef _MEM_REG_DEBUG
#define PRINT_MEM_INFO() \
	g_mr->ShowMemInfo();
#else
#define PRINT_MEM_INFO()
#endif

struct PT_INFO {
	void *pt;
	long nSize;
	time_t tTime;
	int nLine;
	char strFile[256];
};

class TMemReg {

public:
	TMemReg();
	~TMemReg();

	int Init();
	void* Add(void *pt, long size, int line, const char *file);
	void  Del(void *pt);
	void ShowMemInfo();

private:
	TThreadLock *m_pLock;
	map<void *, PT_INFO> m_PtInfo;
};

#endif