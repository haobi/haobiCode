#include "myDef.h"
#include "myfdcntl.h"
#include "mapfile.h"
#ifdef SYSTEM_WIN
	#include <io.h>
#else
	#include <dirent.h>
#endif

size_f GetFileSize(const char *path) {

#ifdef SYSTEM_WIN
	struct _stati64 buf;
#else
	struct stat buf;
#endif

#ifdef SYSTEM_WIN
	if (-1 == _stati64(path, &buf))
#else
	if (-1 == stat(path, &buf))
#endif
		return -1;
	return buf.st_size;
}

size_f GetFileSize(int fd) {

#ifdef SYSTEM_WIN
	struct _stati64 buf;
#else
	struct stat buf;
#endif
	
#ifdef SYSTEM_WIN
	if (-1 == _fstati64(fd, &buf))
#else
	if (-1 == fstat(fd, &buf))
#endif
		return -1;
	return buf.st_size;
}

int DirExist(const char *path) {

	int nRs;
#ifdef SYSTEM_WIN
	struct _stati64 buf;
#else
	struct stat buf;
#endif
	_CHECK_RS(path, NULL, -1);

#ifdef SYSTEM_WIN
	nRs = _stati64(path, &buf);
#else
	nRs = stat(path, &buf);
#endif
	if (-1 == nRs) {
		if (ENOENT == errno)
			return 1;
		return -1;
	}

#ifdef SYSTEM_WIN
	if (_S_IFDIR & buf.st_mode)
#else
	if (S_ISDIR(buf.st_mode))
#endif
		return 0;
	return 1;
}

int FileExist(const char *path) {

	int nRs;
#ifdef SYSTEM_WIN
	struct _stati64 buf;
#else
	struct stat buf;
#endif
	_CHECK_RS(path, NULL, -1);

#ifdef SYSTEM_WIN
	nRs = _stati64(path, &buf);
#else
	nRs = stat(path, &buf);
#endif
	if (-1 == nRs) {
		if (ENOENT == errno)
			return 1;
		return -1;
	}

#ifdef SYSTEM_WIN
	if (_S_IFREG & buf.st_mode)
#else
	if (S_ISREG(buf.st_mode))
#endif
		return 0;
	return 1;
}

int IsDir(const char *path) {

	int nRs;
#ifdef SYSTEM_WIN
	struct _stati64 buf;
#else
	struct stat buf;
#endif

	_CHECK_RS(path, NULL, -1);
	
#ifdef SYSTEM_WIN
	nRs = _stati64(path, &buf);
#else
	nRs = stat(path, &buf);
#endif
	if (-1 == nRs) {
		if (ENOENT == errno)
			return 1;
		return -1;
	}

	#ifdef SYSTEM_WIN
	if (_S_IFDIR & buf.st_mode)
#else
	if (S_ISDIR(buf.st_mode))
#endif
		return 0;
	return 1;
}

#ifdef SYSTEM_WIN
int IsFile(const char *path) {

	DWORD dw;

	_CHECK_RS(path, NULL, -1);

	dw = GetFileAttributesA(path);
	_CHECK_RS(dw, INVALID_FILE_ATTRIBUTES, -1);

	if (FILE_ATTRIBUTE_DIRECTORY & dw)
		return 1;
	return 0;
}
#else
int IsFile(const char *path) {

	struct stat buf;
	if (-1 == stat(path, &buf))
		return -1;

	if (S_ISREG(buf.st_mode))
		return 0;
	return 1;
}
#endif

long WriteFile(const char *path, const void *buffer, long len, int mode) {

	int f;
	long nRs;

	if (0 >= len) {
		_CHECK_RS(-1, -1, -1);
	}
	_CHECK_RS(path, NULL, -1);
	_CHECK_RS(buffer, NULL, -1);

	f = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
	_CHECK_RS(f, -1, -1);
	
	nRs = write(f, buffer, len);
	_CHECK_GOTO(nRs, -1, out);

out:
	close(f);
	return nRs;
}

long ReadFile(const char *path, void *buffer, long len) {
	
	int f;
	long nRs;
	
	if (0 >= len) {
		_CHECK_RS(-1, -1, -1);
	}
	_CHECK_RS(path, NULL, -1);
	_CHECK_RS(buffer, NULL, -1);

	f = open(path, O_RDONLY);
	_CHECK_RS(f, -1, -1);
	
	nRs = read(f, buffer, len);
	_CHECK_GOTO(nRs, -1, out);
	
out:
	close(f);
	return nRs;
}

long AppendFile(const char *path, const void *buffer, long len, int mode) {

	int f;
	long nRs;

	if (0 >= len) {
		_CHECK_RS(-1, -1, -1);
	}
	_CHECK_RS(path, NULL, -1);
	_CHECK_RS(buffer, NULL, -1);

	f = open(path, O_WRONLY | O_APPEND | O_CREAT, mode);
	_CHECK_RS(f, -1, -1);
	
	nRs = write(f, buffer, len);
	_CHECK_GOTO(nRs, -1, out);

out:
	close(f);
	return nRs;
}
//#endif

int MkDir(const char *path, int flag) {

	int i, nRs, nLen;
	char *p = NULL;

	_CHECK_RS(path, NULL, -1);

	nLen = strlen(path);
	p = (char *)calloc(nLen + 2, 1);
	_CHECK_RS(p, NULL, -1);

	strncpy(p, path, nLen + 2);

	if ('/' != p[nLen - 1] || '\\' != p[nLen - 1]) {
		p[nLen] = '/';
		p[nLen + 1] = 0;
	}

	for (i = 0; i < nLen + 2; i++) {
	
		if (('/' == p[i] || '\\' == p[i]) && i > 0) {
			p[i] = 0;
			if (0 != IsDir(p)) {
				// 创建目录
			#ifdef SYSTEM_WIN
				nRs = _mkdir(p);
			#else
				nRs = mkdir(p, flag);
			#endif
				if (-1 == nRs && EEXIST != errno)				
					break ;
			}
			p[i] = '/';
		}
	}

	FREE_PT(p);
	// 创建到最后一级都没有结束，表明此操作是正常的
	if (i != nLen + 2)
		return -1;
	return 0;
}

long GetPageSize() {
#ifdef SYSTEM_WIN
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));

	GetSystemInfo(&si);

	return si.dwPageSize;
#else
	return sysconf(_SC_PAGE_SIZE);
#endif
}

const char* GetExeName(char *path) {

	int i;
	char *p = NULL;
	char strPath[1024] = {0};

	if (NULL == GetFullPathName(strPath)) {
		_CHECK_RS(NULL, NULL, NULL);
	}

	for (i = strlen(strPath); i > 0; i--) {
		if ('\\' == strPath[i] || '/' == strPath[i]) {
			p = strPath + i + 1;
			break ;
		}
	}

	if (NULL == p)
		return NULL;

	strcpy(path, p);
	return path;
}

const char* GetPathName(char *path) {

	int i;
	char strPath[1024] = {0};

	if (NULL == GetFullPathName(strPath)) {
		_CHECK_RS(NULL, NULL, NULL);
	}

	for (i = strlen(strPath); i > 0; i--) {
		if ('\\' == strPath[i] || '/' == strPath[i]) {
			strPath[i] = 0;
			break ;
		}
	}

	if (0 == i)
		return NULL;

	strcpy(path, strPath);
	return path;
}

#ifdef SYSTEM_WIN
const char* GetFullPathName(char *buf) {
	
	char strBuf[1024] = {0};
	HMODULE hProcess;

	hProcess = GetModuleHandle(NULL);
	_CHECK_RS(hProcess, NULL, NULL);

	if (0 == GetModuleFileNameA(hProcess, strBuf, sizeof(strBuf)))
		return NULL;
	strcpy(buf, strBuf);
	return buf;
}
#else
const char* GetFullPathName(char *buf) {

	int rslt;
	char strBuf[1024] = {0};

	rslt = readlink("/proc/self/exe", strBuf, sizeof(strBuf) + 1);
	if (rslt < 0 || rslt >= sizeof(strBuf)) {
		return NULL;
	}

	strBuf[rslt] = 0;
	strcpy(buf, strBuf);
	return buf;
}
#endif

#ifdef SYSTEM_WIN
int CheckOneInstance() {

	char buf[512] = {0};
	const char *p = GetExeName(buf);
	HANDLE hMutex;

	_CHECK_RS(p, NULL, -1);

	hMutex = CreateMutexA(NULL, TRUE, p);
	_CHECK_RS(hMutex, NULL, -1);
	
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		return 1;
	}
	return 0;
}
#else
int CheckOneInstance() {
	
	int fd, nRs;
	bool b;
	char buf[16] = {0};
	char strExeName[256] = {0};
	char strLockFile[512] = {0};
	
	b = false;
	snprintf(strLockFile, sizeof(strLockFile), "%s/%s.pid", 
		LOCK_FILE, GetExeName(strExeName));
	
	fd = open(strLockFile, O_RDWR | O_CREAT, FILE_MODE);
	_CHECK_RS(fd, -1, -1);
	// int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
	if (-1 == lock_reg(fd, 0, F_WRLCK, 0, SEEK_SET, 0)) {
		if (EACCES == errno || EAGAIN == errno) {
			close(fd);
			return 1;
		}
		return -1;
	}
	
	nRs = ftruncate(fd, 0);
	_CHECK_GOTO(nRs, -1, out);
	
	snprintf(buf, sizeof(buf), "%ld", (long)getpid());
	
	nRs = write(fd, buf, strlen(buf));
	_CHECK_GOTO(nRs, -1, out);
	
	b = true;
out:
	// 这里锁了文件后，在程序没有结束前千万不要close，不要锁特性失效
	if (false == b) {
		close(fd);
		return -1;
	}
	return 0;
}
#endif

#ifndef SYSTEM_WIN
int set_fl(int fd, int flags) {

	int val, rs;

	val = fcntl(fd, F_GETFL ,0);
	_CHECK_RS(val, -1, -1);

	val |= flags;

	rs = fcntl(fd, F_SETFL, val);
	_CHECK_RS(rs, -1, -1);

	return 0;
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {

	struct flock lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return (fcntl(fd, cmd, &lock));
}

int file_lock(int fd, int flag) {
	int wait_flag;
	if (0 == flag)
		wait_flag = F_SETLKW;
	else
		wait_flag = F_SETLK;
	return lock_reg(fd, wait_flag, F_WRLCK, 0, SEEK_SET, 0);
}

int file_unlock(int fd) {
	return lock_reg(fd, F_SETLK, F_UNLCK, 0, SEEK_SET, 0);
}

int fd_closeonexec(int fd) {
	return fcntl(fd,F_SETFD,1);
}

bool pipe_init(PIPE_INFO &pi) {
	int rs;
	rs = pipe(pi.fd);
	_CHECK_RS(rs, -1 ,false);
	
	if (-1 == rs)
		pipe_close(pi);
	return true;
}

void pipe_close(const PIPE_INFO &pi) {
	close(pi.fd[0]);
	close(pi.fd[1]);
}

void pipe_write_close(const PIPE_INFO &pi) {
	close(pi.fd[1]);
}

void pipe_read_close(const PIPE_INFO &pi) {
	close(pi.fd[0]);
}

int pipe_write(const PIPE_INFO &pi, const void *buf, int size) {
	return write(pi.fd[1], buf, size);
}

int pipe_read(const PIPE_INFO &pi, void *buf, int size) {
	return read(pi.fd[0], buf, size);
}
#endif

bool MovFile(const char *src, const char *dst) {

#ifdef SYSTEM_WIN
	BOOL bRs;
#else
	int nRs;
#endif
	_CHECK_RS(src, NULL, false);
	_CHECK_RS(dst, NULL, false);
#ifdef SYSTEM_WIN
	bRs = rename(src, dst);
	_CHECK_RS(bRs, FALSE, false);
#else
	nRs = rename(src, dst);
	// 跨文件系统的文件搬迁LINUX下会失败，采取做法：
	// 先复制文件到目标目录，再删除源文件
	if (-1 == nRs) {
		if (EXDEV == errno) {
			bool b;
			b = CpFile(src, dst);
			_CHECK_RS(b, false, false);
			unlink(src);
		}
		else {
			_CHECK_RS(nRs, -1, false);
		}
	}

#endif
	return true;
}

#ifdef SYSTEM_WIN
bool CpFile(const char *src, const char *dst, bool bOverWrite) {

	BOOL bFlag = (true == bOverWrite ? FALSE : TRUE);
	return (TRUE == CopyFileA(src, dst, bFlag) ? true : false);
}
#else
bool CpFile(const char *src, const char *dst, bool bOverWrite) {

	int nRs;
	bool bRs;

redo:
	nRs = link(src, dst);
	if (-1 == nRs) {

		if (EEXIST == errno) {
			if (true == bOverWrite)	 {
				unlink(dst);
				goto redo;	
			}
			else {
				_CHECK_RS(nRs, -1, false);
			}
		}
		else if (EXDEV == errno) {
			bRs = CpFileCrossFileSystem(src, dst, bOverWrite);	
			if (false == bRs) {
				if (ENOMEM == errno) {
					bRs = CpBigFileCrossFileSystem(src, dst, bOverWrite);
					_CHECK_RS(bRs, false, false);
				}
			}
			_CHECK_RS(bRs, false, false);
		}
		else {
			_CHECK_RS(nRs, -1, false);
		}
	}

	return true;
}

bool CpFileCrossFileSystem(const char *src, const char *dst, bool bOverWrite) {

	bool b;
	int f = -1;
	int nRs;
	size_f nSrcSize;
	TMapFile *pSrcFile, *pDstFile;

	b = false;
	pSrcFile = pDstFile = NULL;
	
	pSrcFile = new TMapFile;
	_CHECK_GOTO(pSrcFile, NULL, out);
		
	pDstFile = new TMapFile;
	_CHECK_GOTO(pDstFile, NULL, out);

	nSrcSize = GetFileSize(src);
	_CHECK_GOTO(nSrcSize, -1, out);
	// 创建目标文件并截长度
	if (true == bOverWrite) {
		f = open(dst, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
		_CHECK_GOTO(f, -1, out);
	}
	else {
		f = open(dst, O_WRONLY | O_CREAT | O_EXCL, FILE_MODE);
		_CHECK_GOTO(f, -1, out);
	}

	nRs = ftruncate(f, nSrcSize);
	_CHECK_GOTO(nRs, -1, out);

	nRs = pSrcFile->Open(src);
	_CHECK_GOTO(nRs, -1, out);

	nRs = pDstFile->Open(dst);
	_CHECK_GOTO(nRs, -1, out);

	memcpy(pDstFile->Addr(), pSrcFile->Addr(), pSrcFile->Size());

	b = true;
out:
	if (false == b)
		unlink(dst);

	RELEASE_PT(pSrcFile);
	RELEASE_PT(pDstFile);
	CLOSE_F(f);

	return b;
}

bool CpBigFileCrossFileSystem(const char *src, const char *dst, bool bOverWrite) {

	bool b;
	int nRSize, nWSize, nBufSize;
	int nSrcF, nDstF;
	char *pBuf;

	b = false;
	nBufSize = SIZE_1M;
	nSrcF = nDstF = -1;
	pBuf = NULL;

	pBuf = (char *)calloc(nBufSize, 1);
	_CHECK_RS(pBuf, NULL, -1);

	nSrcF = open(src, O_RDONLY);
	_CHECK_GOTO(nSrcF, -1, out);

	if (true == bOverWrite) {
		nDstF = open(dst, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
		_CHECK_GOTO(nDstF, -1, out);
	}
	else {
		nDstF = open(dst, O_WRONLY | O_CREAT | O_EXCL, FILE_MODE);
		_CHECK_GOTO(nDstF, -1, out);
	}

	nRSize = read(nSrcF, pBuf, nBufSize);
	_CHECK_GOTO(nRSize, -1, out);

	do {
		
		nWSize = write(nDstF, pBuf, nRSize);
		_CHECK_GOTO(nWSize, -1, out);

		nRSize = read(nSrcF, pBuf, nBufSize);
		_CHECK_GOTO(nRSize, -1, out);
	} while (0 != nRSize);

	b = true;
out:
	if (false == b) {
		unlink(dst);
	}

	CLOSE_F(nSrcF);
	CLOSE_F(nDstF);
	FREE_PT(pBuf);

	return b;
}
#endif

bool DelFile(const char *path) {

#ifdef SYSTEM_WIN
	BOOL bRs;
#else
	int nRs;
#endif
	_CHECK_RS(path, NULL, false);
	
#ifdef SYSTEM_WIN
	bRs = DeleteFileA(path);
	_CHECK_RS(bRs, FALSE, false);
#else
	nRs = unlink(path);
	_CHECK_RS(nRs, -1, false);
#endif
	return true;
}

bool DelDir(const char *path) {

	int nRs;
	bool bRs, b;
	char strPath[512] = {0};
#ifdef SYSTEM_WIN
	long iFind;
	_finddata_t fileinfo;
#else
	DIR *dp;
	struct dirent *dirp;
#endif

	_CHECK_RS(path, NULL, false);
#ifdef SYSTEM_WIN
	memset(&fileinfo, 0, sizeof(fileinfo));
	snprintf(strPath, sizeof(strPath), "%s/*.*", path);

	iFind = _findfirst(strPath, &fileinfo);
	_CHECK_RS(iFind, -1, false);
#else
	strncpy(strPath, path, sizeof(strPath));

	dp = opendir(strPath);
	_CHECK_RS(dp, NULL, false);
#endif

	b = false;
#ifdef SYSTEM_WIN
	while (0 == _findnext(iFind, &fileinfo)) {
		if ('.' == fileinfo.name[0])
			continue ;
		snprintf(strPath, sizeof(strPath), "%s/%s", path, fileinfo.name);
#else
	while (NULL != (dirp = readdir(dp))) {
		if ('.' == dirp->d_name[0])
			continue ;
		snprintf(strPath, sizeof(strPath), "%s/%s", path, dirp->d_name);
#endif
		nRs = IsDir(strPath);
		if (0 == nRs) {
			bRs = DelDir(strPath);
			_CHECK_GOTO(bRs, false, out);
		}
		else if (1 == nRs) {

			bRs = DelFile(strPath);
			_CHECK_GOTO(bRs, false, out);
		}
	}
	
	b = true;
out:
#ifdef SYSTEM_WIN
	_findclose(iFind);
#else
	closedir(dp);
#endif
	
	if (false == b)
		return false;
#ifdef SYSTEM_WIN

	if (FALSE == RemoveDirectoryA(path)) {
#else
	if (-1 == rmdir(path)) {
#endif
		return false;
	}
	return true;
}
