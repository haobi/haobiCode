/*
�ļ����ƣ�myfdcntl.h
����������Ҫ�Ƿ�װ�ճ��ļ򵥵��ļ���Ŀ¼�Ĳ���
���ߣ�hejm
�汾����ʼ�汾(������)
*/
#ifndef _MY_FD_CNTL_H
#define _MY_FD_CNTL_H

#include "myDef.h"

// ��������ļ���
#ifdef SYSTEM_WIN
//	#define S_ISREG _S_IFREG
//	#define S_ISDIR _S_IFDIR

	#define open  _open
	#define close _close
	#define write _write
	#define read  _read

	#define unlink _unlink
#endif

#ifdef SYSTEM_WIN
	#define FILE_MODE _S_IREAD | _S_IWRITE
	
	#define O_WRONLY _O_WRONLY
	#define O_RDWR   _O_RDWR
	#define O_TRUNC  _O_TRUNC
	#define O_APPEND _O_APPEND
	#define O_CREAT  _O_CREAT
	#define O_EXCL   _O_EXCL
#else
	#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
	#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
#endif

#define SIZE_1M (1024 * 1024)

size_f GetFileSize(const char *path);
size_f GetFileSize(int fd);

int DirExist(const char *path);

int FileExist(const char *path);

int IsDir(const char *path);
/* linux���Ǳ�ʶ������ͨ�ļ� */
int IsFile(const char *path);
/* 32λ����������£���Ҫһ���Բ������������2GB��С���ļ� */
long WriteFile(const char *path, const void *buffer, long len, int mode = FILE_MODE);

long ReadFile(const char *path,void *buffer,long len);

long AppendFile(const char *path,const void *buffer,long len,int mode = FILE_MODE);

int MkDir(const char *path, int flag = 0755);

long GetPageSize();

const char* GetExeName(char *buf);

const char* GetPathName(char *buf);

const char* GetFullPathName(char *buf);

/*
����ֵ��
0�������ڸó���ʵ��
1�����ڸó���ʵ��
-1���������г���
*/
int CheckOneInstance();

#ifndef SYSTEM_WIN
//--------------------------------------------
//			LINUX���ļ��������Ĳ���
//--------------------------------------------
int set_fl(int fd,int flags);
int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len);
/* ��������ļ�����0Ϊ����������Ϊ������ */
int file_lock(int fd,int flag = 0);
int file_unlock(int fd);
int fd_closeonexec(int fd);

//--------------------------------------------
//			LINUX�ܵ�����
//--------------------------------------------
struct PIPE_INFO {
	int fd[2];
};

bool pipe_init(PIPE_INFO &pi);
void pipe_close(const PIPE_INFO &pi);
void pipe_write_close(const PIPE_INFO &pi);
void pipe_read_close(const PIPE_INFO &pi);
int pipe_write(const PIPE_INFO &pi, const void *buf, int size);
int pipe_read(const PIPE_INFO &pi, void *buf, int size);
/*
// ������ʱû��ʵ��socket�����Ŀ��ļ����ù�����ʱ����
// ����ֵ��-1����0��ǰfd���ɶ���1��ǰfd�ɶ�
int pipe_can_read(const PIPE_INFO &pi, struct timeval *t = NULL);
*/
#endif

//--------------------------------------------
//				���ļ�����
//--------------------------------------------
// MoveFile�����ƶ��ļ���Ŀ¼
bool MovFile(const char *src, const char *dst);
// CopyFileֻ�ܸ����ļ������ܸ���Ŀ¼
// linux�˵ĸ���Ҫ���Կ��ļ�ϵͳ�ģ���ֻ�ܲ�ȡд�ļ��ķ�ʽ
bool CpFile(const char *src, const char *dst, bool bOverWrite = true);

#ifndef SYSTEM_WIN
bool CpFileCrossFileSystem(const char *src, const char *dst, bool bOverWrite = true);
// ��Բ����û����ڴ治�㣬�������ļ�ʱ(��1G)�����errno=ENOMEM�Ĵ���
bool CpBigFileCrossFileSystem(const char *src, const char *dst, bool bOverWrite = true);
#endif

// DelFile����ɾĿ¼
bool DelFile(const char *path);

bool DelDir(const char *path);

#endif
