#ifndef _MY_SHM_H
#define _MY_SHM_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifndef SYSTEM_WIN

#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

class TShm {

public:
	TShm();
	~TShm();

	int ShmInit(key_t key, long size, int flag = 0644);
	int ShmConnect(key_t key);
	int ShmReconnect();
	int ShmDisconnect();
	// ע��Ҫ������״̬�²���ɾ�������ڴ��
	int ShmDelete();
	// ��������չʧ�ܵ��µ����ݶ�ʧ��������ȥ�е�
	int ShmExtend(long size);
	
	inline void* GetAddr() {return m_Addr;}
	inline key_t GetKey() {return m_key;}
	long GetSize();
	int GetFlag();

private:
	key_t m_key;
	int m_shmid;
	void *m_Addr;
};

#endif
#endif
