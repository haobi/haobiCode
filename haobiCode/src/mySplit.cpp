#include "mySplit.h"

TSplit::TSplit() {
	m_strBuf = NULL;
	m_Array = NULL;
	m_nBufLen = 0;
	m_nCnt = 0;
	m_nTotalCnt = 0;
}

TSplit::~TSplit() {
	if (m_strBuf)
		FREE_PT(m_strBuf);
	if (m_Array)
		FREE_PT(m_Array);
}

int TSplit::split(const char *token, const char *buf) {

    _CHECK_RS(token,NULL,-1);
    _CHECK_RS(buf,NULL,-1);

    char *pos = NULL;
    char *buf_pt = NULL;
    int buf_len = strlen(buf) + 1;
    int token_len = strlen(token);

	if (m_nBufLen < buf_len) {
		if (m_strBuf) {
			FREE_PT(m_strBuf);
		}
		m_strBuf = (char *)calloc(buf_len, 1);
		_CHECK_RS(m_strBuf, NULL, -1);
	}
	else {
		memset(m_strBuf, 0, m_nBufLen);
	}

	m_nCnt = 0;

    memcpy(m_strBuf, buf, buf_len);

    buf_pt = m_strBuf;
    while (1) {

		if (0 == buf_pt[0]) {
			if (0 < m_nCnt)
				m_Array[m_nCnt++] = (PVOID)"";
			break;
		}
        
        if (CheckExtend())
            return -1; 

        pos = strstr(buf_pt, token);
        if (!pos) {
            m_Array[m_nCnt++] = buf_pt;
            break;
        }   
        *pos = 0;
        if (pos != buf_pt)
            m_Array[m_nCnt++] = buf_pt;
		else
			m_Array[m_nCnt++] = (PVOID)"";
        buf_pt = pos + token_len;
    }   
    return 0;
}

int TSplit::CheckExtend() {
	if (m_nTotalCnt <= m_nCnt) {
		int nAllocLen = m_nTotalCnt == 0 ? SPLIT_LEN : m_nTotalCnt * 2;
//		m_Array = _REALLOC_SZ(m_Array, PVOID, nAllocLen);
		m_Array = (PVOID *)realloc(m_Array, sizeof(PVOID) * nAllocLen);
		_CHECK_RS(m_Array, NULL, -1);
		m_nTotalCnt = nAllocLen;
	}
	return 0;
}

const char* TSplit::operator[](int pos) const {
	if (0 > pos && pos >= m_nCnt)
		return NULL;
	return (const char *)m_Array[pos];
}

int TSplit::Count() const {
	return m_nCnt;
}
