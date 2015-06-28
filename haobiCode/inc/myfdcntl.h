/*
文件名称：myfdcntl.h
简单描述：主要是封装日常的简单的文件、目录的操作
作者：hejm
版本：初始版本(开发中)
*/
#ifndef _MY_FD_CNTL_H
#define _MY_FD_CNTL_H

#include "myDef.h"

// 函数方面的兼容
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
/* linux下是标识的是普通文件 */
int IsFile(const char *path);
/* 32位机器的情况下，不要一次性操作超过或等于2GB大小的文件 */
long WriteFile(const char *path, const void *buffer, long len, int mode = FILE_MODE);

long ReadFile(const char *path,void *buffer,long len);

long AppendFile(const char *path,const void *buffer,long len,int mode = FILE_MODE);

int MkDir(const char *path, int flag = 0755);

long GetPageSize();

const char* GetExeName(char *buf);

const char* GetPathName(char *buf);

const char* GetFullPathName(char *buf);

/*
返回值：
0：不存在该程序实例
1：存在该程序实例
-1：函数运行出错
*/
int CheckOneInstance();

#ifndef SYSTEM_WIN
//--------------------------------------------
//			LINUX对文件描述符的操作
//--------------------------------------------
int set_fl(int fd,int flags);
int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len);
/* 阻塞版的文件锁，0为阻塞，否则为非阻塞 */
int file_lock(int fd,int flag = 0);
int file_unlock(int fd);
int fd_closeonexec(int fd);

//--------------------------------------------
//			LINUX管道操作
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
// 由于暂时没有实现socket操作的库文件，该功能暂时屏蔽
// 返回值：-1出错、0当前fd不可读、1当前fd可读
int pipe_can_read(const PIPE_INFO &pi, struct timeval *t = NULL);
*/
#endif

//--------------------------------------------
//				简单文件操作
//--------------------------------------------
// MoveFile可以移动文件或目录
bool MovFile(const char *src, const char *dst);
// CopyFile只能复制文件，不能复制目录
// linux端的复制要可以跨文件系统的，故只能采取写文件的方式
bool CpFile(const char *src, const char *dst, bool bOverWrite = true);

#ifndef SYSTEM_WIN
bool CpFileCrossFileSystem(const char *src, const char *dst, bool bOverWrite = true);
// 针对部分用户的内存不足，拷贝大文件时(如1G)会出现errno=ENOMEM的错误
bool CpBigFileCrossFileSystem(const char *src, const char *dst, bool bOverWrite = true);
#endif

// DelFile不能删目录
bool DelFile(const char *path);

bool DelDir(const char *path);

#endif
