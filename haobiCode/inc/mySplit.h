#ifndef _SPLIT_H
#define _SPLIT_H

#include "myDef.h"

#define SPLIT_LEN 100

class TSplit {
public:
    TSplit();
    ~TSplit();

 	// token分隔符
	int split(const char *token, const char *buf);
	int Count() const;

    const char* operator[](int pos) const;
private:
	TSplit(const TSplit &sp);
	TSplit& operator=(const TSplit &sp);

    int CheckExtend();
private:
	char *m_strBuf;
    PVOID *m_Array;
	int m_nBufLen;
    int m_nCnt;			// 已经使用的大小
    int m_nTotalCnt;	// 总共可以使用的大小
};

#endif
