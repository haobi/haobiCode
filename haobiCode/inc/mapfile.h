#ifndef _MAP_FILE_H
#define _MAP_FILE_H

//#define SYSTEM_WIN

#ifdef SYSTEM_WIN
#include <windows.h>
#else
#include <sys/mman.h>
#endif
/*
注意：这里32位机器最多映射文件为2GB - 1byte，64位机器暂时不需要限制
*/

#define MAX_MAP_PATH 512

class TMapFile {
public:
	TMapFile();
	~TMapFile();
	
	int Open(const char *path);
	int Close();
	int Flush();
	int Extend(long size);

	int Flush(void *addr, long size);

	long Size() {return m_nSize;}
	void* Addr() {return m_Addr;}
	const char* GetFileName() {return m_strPath;}

	enum {
		OP_FAIL = -1,
		OP_SUCCEED,	
	};
private:
#ifdef SYSTEM_WIN
	int Open4Win();
	int Extend4Win(long size);
#else
	int Open4Linux();
	int Extend4Linux(long size);
#endif

#ifdef SYSTEM_WIN
	HANDLE m_hFile;
	HANDLE m_hMapFile;
#else
	int m_fd;
#endif

	char m_strPath[MAX_MAP_PATH];
	long m_nSize;
	void *m_Addr;
};

#endif