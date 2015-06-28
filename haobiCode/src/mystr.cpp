#include "myDef.h"
#include "mystr.h"

char* ltrim(char *p) {

	int len,i;

	_CHECK_RS(p, NULL, NULL);

	len = strlen(p);
	for (i = 0; i < len; i++) {
		if (' ' == p[i] || '\t' == p[i])
			;
		else
			break;
	}

	if (0 != i) {
		memmove(p, p + i, len - i);
		p[len - i] = 0;
	}

	return p;
}

char* rtrim(char *p) {

	int len;

	_CHECK_RS(p, NULL, NULL);

	len = strlen(p);
	for (int i = len - 1; i >= 0; i--) {
		if (' ' == p[i] || '\t' == p[i]) {
			p[i] = 0;
		}
		else
			break;
	}
	return p;
}

char* trim(char *p) {

	char *buf;

	_CHECK_RS(p, NULL, NULL);

	buf = NULL;

	buf = ltrim(p);
	_CHECK_RS(buf, NULL, NULL);
	
	buf = rtrim(buf);
	_CHECK_RS(buf, NULL, NULL);

	return buf;	
}

char* chomp(char *p) {

	int len;

	_CHECK_RS(p, NULL, NULL);

	len = strlen(p);

	if ('\n' == p[len - 1])
		p[len - 1] = 0;

	if ('\r' == p[len - 2])
		p[len - 2] = 0;

	return p;
}

char* strreplace(const char *strText, const char *strSrc, const char *strDst) {

	int nLeft;
	int nTextLen;
	int nSrcLen;
	int nDstLen;
	int nReplaceLen;
	int nPos; // 目前目标输出缓存指针
	bool b;

	char *strBuf;
	const char *pBeg, *p;
	
	static int nReplaceDefCnt = 1024; // 默认的字符串缓存长度

	_CHECK_RS(strText, NULL, NULL);
	_CHECK_RS(strSrc, NULL, NULL);
	_CHECK_RS(strDst, NULL, NULL);

	nTextLen = strlen(strText);
	nSrcLen = strlen(strSrc);
	nDstLen = strlen(strDst);
	nPos = 0;
	b = false;

	// 预计缓存长度
	if (nReplaceDefCnt < nTextLen * 2) {
		// 预计两倍的缓存长度	
		nReplaceLen = nTextLen * 2;
	}
	else {
		nReplaceLen = nReplaceDefCnt;
	}

	strBuf = (char *)calloc(nReplaceLen, 1);
	_CHECK_RS(strBuf, NULL, NULL);

	pBeg = strText;
	while (p = strstr(pBeg, strSrc)) {

		int nTmpLen = (int)(p - pBeg);
		// 判断拷贝的字符串长度是否大于剩余缓存的长度，如有必要则重新分配缓存
		if ((nTmpLen + nDstLen) >= (nReplaceLen - nPos)) {
			// 必须保证该次分配的内存是足够这次拷贝使用的
			// 分配原则：(当前缓存长度 + 截取到的字符串长度 + 要替换的字符串长度) * 2
			nReplaceLen = (nReplaceLen + nTmpLen + nDstLen) * 2;
			strBuf = (char *)realloc(strBuf, nReplaceLen);
			_CHECK_GOTO(strBuf, NULL, out);
		}
		
		// 拷贝字符串
		if (0 != nTmpLen) {
			memcpy(strBuf + nPos, pBeg, p - pBeg);
			nPos += nTmpLen;
		}

		// 拷贝替换的字符串
		memcpy(strBuf + nPos, strDst, nDstLen);
		nPos += nDstLen;

		pBeg = p + nSrcLen;
	}

	// 将剩余的字符串拷贝
	nLeft = strlen(pBeg);
	if (0 < nLeft) {
		if (nReplaceLen - nPos <= nLeft) {
			// 保证重新分配的缓存的空间是足够的
			nReplaceLen = nReplaceLen + nLeft + 1;
			strBuf = (char *)realloc(strBuf, nReplaceLen);
			_CHECK_GOTO(strBuf, NULL, out);
		}

		memcpy(strBuf + nPos, pBeg, nLeft);
	}
	
	strBuf[nPos + nLeft] = 0;

	b = true;
out:
	if (false == b) {
		FREE_PT(strBuf);
		return NULL;
	}

	return strBuf;
}

bool IsStrDigit(const char *strText) {

	for (int i = 0; i < strlen(strText); i++) {
		if (!is_digit(strText[i]))
			return false;
	}
	return true;
}

bool IsStrLetter(const char *strText) {

	for (int i = 0; i < strlen(strText); i++) {
		if (!is_letter(strText[i]))
			return false;
	}
	return true;
}