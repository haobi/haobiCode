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
	int nPos; // ĿǰĿ���������ָ��
	bool b;

	char *strBuf;
	const char *pBeg, *p;
	
	static int nReplaceDefCnt = 1024; // Ĭ�ϵ��ַ������泤��

	_CHECK_RS(strText, NULL, NULL);
	_CHECK_RS(strSrc, NULL, NULL);
	_CHECK_RS(strDst, NULL, NULL);

	nTextLen = strlen(strText);
	nSrcLen = strlen(strSrc);
	nDstLen = strlen(strDst);
	nPos = 0;
	b = false;

	// Ԥ�ƻ��泤��
	if (nReplaceDefCnt < nTextLen * 2) {
		// Ԥ�������Ļ��泤��	
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
		// �жϿ������ַ��������Ƿ����ʣ�໺��ĳ��ȣ����б�Ҫ�����·��仺��
		if ((nTmpLen + nDstLen) >= (nReplaceLen - nPos)) {
			// ���뱣֤�ôη�����ڴ����㹻��ο���ʹ�õ�
			// ����ԭ��(��ǰ���泤�� + ��ȡ�����ַ������� + Ҫ�滻���ַ�������) * 2
			nReplaceLen = (nReplaceLen + nTmpLen + nDstLen) * 2;
			strBuf = (char *)realloc(strBuf, nReplaceLen);
			_CHECK_GOTO(strBuf, NULL, out);
		}
		
		// �����ַ���
		if (0 != nTmpLen) {
			memcpy(strBuf + nPos, pBeg, p - pBeg);
			nPos += nTmpLen;
		}

		// �����滻���ַ���
		memcpy(strBuf + nPos, strDst, nDstLen);
		nPos += nDstLen;

		pBeg = p + nSrcLen;
	}

	// ��ʣ����ַ�������
	nLeft = strlen(pBeg);
	if (0 < nLeft) {
		if (nReplaceLen - nPos <= nLeft) {
			// ��֤���·���Ļ���Ŀռ����㹻��
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