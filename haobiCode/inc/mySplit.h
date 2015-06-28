#ifndef _SPLIT_H
#define _SPLIT_H

#include "myDef.h"

#define SPLIT_LEN 100

class TSplit {
public:
    TSplit();
    ~TSplit();

 	// token�ָ���
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
    int m_nCnt;			// �Ѿ�ʹ�õĴ�С
    int m_nTotalCnt;	// �ܹ�����ʹ�õĴ�С
};

#endif
