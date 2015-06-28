#include "myDef.h"
#include "myfdcntl.h"
#include "mapfile.h"

TMapFile::TMapFile() {
	memset(m_strPath, 0, sizeof(m_strPath));
	m_nSize = 0;
	m_Addr = NULL;
#ifdef SYSTEM_WIN
	m_hFile = NULL;
	m_hMapFile = NULL;
#else
	m_fd = -1;
#endif
}

TMapFile::~TMapFile() {
	Close();
}

int TMapFile::Open(const char *path) {

	int nRs;
	size_f size;

	_CHECK_RS(path, NULL, OP_FAIL);
	/* 重新打开之前要将之前的映射操作释放 */
	Close();

	strncpy(m_strPath, path, sizeof(m_strPath));
	size = GetFileSize(m_strPath);
	_CHECK_RS(size, -1, OP_FAIL);
	/* 32位机器不建议映射超过2GB的文件 */
	if (true == BIT_32 && true == OverNGB(size, 2)) {
		_CHECK_RS(-1, -1, OP_FAIL);
	}
	
#ifdef SYSTEM_WIN
	nRs = Open4Win();
#else
	nRs = Open4Linux();
#endif
	_CHECK_RS(nRs, OP_FAIL, OP_FAIL);

	return OP_SUCCEED;
}

int TMapFile::Close() {

#ifdef SYSTEM_WIN
	if (m_Addr) {
		UnmapViewOfFile(m_Addr);
	}
	CLOSE_HANDLE(m_hMapFile);
	CLOSE_HANDLE(m_hFile);
#else
	int nRs;
	if (m_Addr) {
		munmap(m_Addr, m_nSize);
	}
	
	CLOSE_F(m_fd);
#endif
	m_Addr = NULL;
	m_nSize = 0;

	return OP_SUCCEED;
}

int TMapFile::Flush() {
	return Flush(m_Addr, m_nSize);
}

int TMapFile::Extend(long size) {
	
	int nRs;

	if (0 >= size || size <= m_nSize)
		return OP_FAIL;

	if (true == BIT_32 && true == OverNGB(size, 2))
		return OP_FAIL;

#ifdef SYSTEM_WIN
	nRs = Extend4Win(size);
#else
	nRs = Extend4Linux(size);
#endif
	_CHECK_RS(nRs, OP_FAIL, OP_FAIL);

	return nRs;
}

int TMapFile::Flush(void *addr, long size) {

#ifdef SYSTEM_WIN
	BOOL bRs;
	bRs = FlushViewOfFile(addr, size);
	_CHECK_RS(bRs, FALSE, OP_FAIL);
#else
	int nRs;
	nRs = munmap(addr, size);
	_CHECK_RS(nRs, -1, OP_FAIL);
#endif

	return OP_SUCCEED;
}

#ifdef SYSTEM_WIN
int TMapFile::Open4Win() {
	
	bool b;
	void *addr;
	DWORD dwSize;
	HANDLE hFile, hMapFile;

	b = false;
	addr = NULL;
	hFile = hMapFile = NULL;

	hFile = CreateFileA(
		m_strPath, 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	_CHECK_GOTO(hFile, INVALID_HANDLE_VALUE, out);

	dwSize = GetFileSize(hFile, NULL);
	_CHECK_GOTO(dwSize, 0xFFFFFFFF, out);

	hMapFile = CreateFileMapping(
		hFile,
		NULL,
		PAGE_READWRITE,
		0,
		/*dwSize*/0,
		NULL);
	_CHECK_GOTO(hMapFile, INVALID_HANDLE_VALUE, out);
	
	addr = MapViewOfFile(
		hMapFile, 
		FILE_MAP_READ | FILE_MAP_WRITE, 
		0,
		0,
		0);
	_CHECK_GOTO(addr, NULL, out);

	b = true;

out:
	if (false == b) {
		CLOSE_HANDLE(hFile);
		CLOSE_HANDLE(hMapFile);
		_CHECK_RS(-1, -1, OP_FAIL);
	}
	else {
		m_hFile = hFile;
		m_hMapFile = hMapFile;
		m_Addr = addr;
		m_nSize = dwSize;
	}

	return OP_SUCCEED;
}

int TMapFile::Extend4Win(long size) {
	
	BOOL bRs;
	DWORD dwWritten, dw;

	int nRs;

	bRs = UnmapViewOfFile(m_Addr);
	_CHECK_RS(bRs, FALSE, OP_FAIL);

	CLOSE_HANDLE(m_hMapFile);

	dw = SetFilePointer(m_hFile, size - 1, NULL, FILE_BEGIN);
	_CHECK_RS(dw, INVALID_SET_FILE_POINTER, OP_FAIL);

	bRs = WriteFile(m_hFile, "\0", 1, &dwWritten, NULL);
	_CHECK_RS(bRs, FALSE, OP_FAIL);

	if (1 != dwWritten) {
		_CHECK_RS(-1, -1, OP_FAIL);
	}

	nRs = Open(m_strPath);
	_CHECK_RS(nRs, OP_FAIL, OP_FAIL);

	return OP_SUCCEED;
}
#else 
int TMapFile::Open4Linux() {

	int fd;
	bool b;
	long nSize;
	void *addr;

	b = false;
	fd = -1;

	fd = open(m_strPath, O_RDWR);
	_CHECK_RS(fd, -1, -1);

	nSize = GetFileSize(fd);
	_CHECK_GOTO(nSize, -1, out);

	addr = mmap((void *)0, nSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	_CHECK_GOTO(addr, (void *)-1, out);

	b = true;
out:

	if (false == b) {
		CLOSE_F(fd);
		return OP_FAIL;
	}
	else {
		m_fd = fd;
		m_nSize = nSize;
		m_Addr = addr;
	}

	return OP_SUCCEED;
}

int TMapFile::Extend4Linux(long size) {

	int nRs;
	void *addr;

	nRs = munmap(m_Addr, m_nSize);
	_CHECK_RS(nRs, -1, OP_FAIL);

	nRs = ftruncate(m_fd, size);
	_CHECK_RS(nRs, -1, OP_FAIL);
/*
	addr = mmap((void *)0, size, PROT_WRITE | PROT_READ, MAP_SHARED, m_fd, 0);
	_CHECK_RS(addr, (void *)-1, OP_FAIL);

	m_nSize = size;
	m_Addr = addr;
*/

	nRs = Open(m_strPath);
	_CHECK_RS(nRs, OP_FAIL, OP_FAIL);

	return OP_SUCCEED;
}
#endif
