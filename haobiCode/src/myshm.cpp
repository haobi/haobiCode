#include "myshm.h"
#include "myDef.h"

#ifndef SYSTEM_WIN

TShm::TShm() {

	m_key = -1;
	m_shmid = -1;
	m_Addr = NULL;
}

TShm::~TShm() {
	ShmDisconnect();
}

int TShm::ShmInit(key_t key, long size, int flag) { 

	int shmid;
	void *addr;

	if (0 >= size || 0 > flag) {
		_CHECK_RS(-1, -1, -1);
	}
	/* 已经连接了的，要先进行释放先 */
	if (-1 != m_shmid) {
		_CHECK_RS(-1, -1, -1);
	}
	
	shmid = shmget(key, size, flag | IPC_CREAT | IPC_EXCL);	
	if (-1 == shmid) {
		if (EEXIST == errno) {
			shmid = shmget(key, 0, IPC_EXCL);	
			_CHECK_RS(shmid, -1, -1);
		}
		else {
			_CHECK_RS(shmid, -1, -1);
		}
	}

	addr = shmat(shmid, NULL, 0);
	_CHECK_RS(addr, (void *)-1, -1);

	m_key = key;
	m_shmid = shmid;
	m_Addr = addr;

	return 0;
}

int TShm::ShmConnect(key_t key) {

	int shmid;
	void *addr;
	
	if (-1 != m_shmid) {
		_CHECK_RS(-1, -1, -1);
	}
	
	shmid = shmget(key, 0, IPC_EXCL);
	_CHECK_RS(shmid, -1, -1);

	addr = shmat(shmid, NULL, 0);
	_CHECK_RS(addr, (void *)-1, -1);

	m_key = key;
	m_shmid = shmid;
	m_Addr = addr;

	return 0;
}

int TShm::ShmReconnect() {

	int shmid;
	void *addr;
	
	ShmDisconnect();
	
	shmid = shmget(m_key, 0, IPC_EXCL);
	_CHECK_RS(shmid, -1, -1);

	addr = shmat(shmid, NULL, 0);
	_CHECK_RS(addr, (void *)-1, -1);

	m_shmid = shmid;
	m_Addr = addr;

	return 0;
}

int TShm::ShmDisconnect() {

	int nRs;
	
	if (NULL == m_Addr || -1 == m_shmid || -1 == m_key)
		return 0;

	nRs = shmdt(m_Addr);
	_CHECK_RS(nRs, -1, -1);

	m_Addr = NULL;
	m_shmid = -1;

	return 0;
}

int TShm::ShmDelete() {

	int nRs;
	
	_CHECK_RS(m_shmid, -1, -1);
	_CHECK_RS(m_Addr, NULL, -1);

	nRs = shmdt(m_Addr);
	_CHECK_RS(nRs, -1, -1);

	nRs = shmctl(m_shmid, IPC_RMID, 0);
	_CHECK_RS(nRs, -1, -1);

	m_Addr = NULL;
	m_shmid = -1;
//	m_key = -1;

	return 0;
}

int TShm::ShmExtend(long size) {

	int nRs, nRes, nFlag;
	long nOldSize;
	char *Addr;

	_CHECK_RS(m_Addr, NULL, -1);
	_CHECK_RS(m_shmid, -1, -1);
	_CHECK_RS(m_key, -1, -1);

	nRes = -1;

	nFlag = GetFlag();
	nOldSize = GetSize();

	if (nOldSize >= size) {
		_CHECK_RS(-1, -1, -1);
	}
	
	Addr = (char *)calloc(nOldSize, 1);
	_CHECK_RS(Addr, NULL, -1);

	memcpy(Addr, m_Addr, nOldSize);

	nRs = ShmDelete();
	_CHECK_GOTO(nRs, -1, out);

	nRs = ShmInit(m_key, size, nFlag);
	_CHECK_GOTO(nRs, -1, out);

	memcpy(m_Addr, Addr, nOldSize);
	nRes = 0;

out:
	FREE_PT(Addr);

	return nRes;
}

long TShm::GetSize() {

	int nRs;
	struct shmid_ds buf;

	_CHECK_RS(m_Addr, NULL, -1);
	_CHECK_RS(m_shmid, -1, -1);
	_CHECK_RS(m_key, -1, -1);

	nRs = shmctl(m_shmid, IPC_STAT, &buf);
	_CHECK_RS(nRs, -1, -1);
	
	return buf.shm_segsz;
}

int TShm::GetFlag() {

	int nRs;
	struct shmid_ds buf;

	_CHECK_RS(m_Addr, NULL, -1);
	_CHECK_RS(m_shmid, -1, -1);
	_CHECK_RS(m_key, -1, -1);

	nRs = shmctl(m_shmid, IPC_STAT, &buf);
	_CHECK_RS(nRs, -1, -1);
	
	return buf.shm_perm.mode;	
}

#endif