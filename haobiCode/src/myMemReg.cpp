#include "myMemReg.h"

TMemReg::TMemReg() {
	m_pLock = NULL;
}

TMemReg::~TMemReg() {
	RELEASE_PT(m_pLock);
}

int TMemReg::Init() {

	int nRs;

	m_pLock = new TThreadLock;
	_CHECK_RS(m_pLock, NULL, -1);

	nRs = m_pLock->Init();
	_CHECK_RS(nRs, -1, -1);

	return 0;
}

void* TMemReg::Add(void *pt, long size, int line, const char *file) {

	PT_INFO pi;
	map<void *, PT_INFO>::iterator it;

	_CHECK_RS(pt, NULL, NULL);
	_CHECK_RS(file, NULL, NULL);

	if (!m_pLock) {
		m_pLock = new TThreadLock;
	}

	memset(&pi, 0, sizeof(pi));

	m_pLock->Lock();

	it = m_PtInfo.find(pt);
	if (m_PtInfo.end() == it) {
		
		pi.pt = pt;
		pi.nSize = size;
		pi.tTime = time(NULL);
		pi.nLine = line;
		strncpy(pi.strFile, file, sizeof(pi.strFile));
		
		m_PtInfo.insert(make_pair(pt, pi));
	}
	
	m_pLock->UnLock();

	return pt;
}

void TMemReg::Del(void *pt) {

	map<void *, PT_INFO>::iterator it;

	m_pLock->Lock();

	it = m_PtInfo.find(pt);
	if (m_PtInfo.end() == it)
		return ;

	m_PtInfo.erase(it);

	m_pLock->UnLock();
}

void TMemReg::ShowMemInfo() {

	int nNum;
	char buf[512], strTime[32];
	map<void *, PT_INFO>::iterator it;

	m_pLock->Lock();

	nNum = 0;
	for (it = m_PtInfo.begin(); it != m_PtInfo.end(); it++) {
		
		PT_INFO &pi = it->second;
		time2str(pi.tTime, strTime);
		snprintf(buf, sizeof(buf), "[%d] addr[%p], size[%ld], time[%s], line[%d], file[%s]", 
			nNum++, pi.pt, pi.nSize, strTime, pi.nLine, pi.strFile);
		printf("%s\n", buf);
	}

	m_pLock->UnLock();
}