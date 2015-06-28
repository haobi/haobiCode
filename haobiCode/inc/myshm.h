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
	// 注意要在连接状态下才能删除共享内存块
	int ShmDelete();
	// 关于因扩展失败导致的数据丢失，调用者去承担
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
