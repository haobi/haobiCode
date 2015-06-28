#ifndef _FILE_SEARCH_H
#define _FILE_SEARCH_H

#include <stdio.h>
#include <stdlib.h>

class TFileSearchRule {

	struct FILE_INFO {
		char strPath[256];
		char strFileName[128];
	};

public:
	TFileSearchRule();	
	~TFileSearchRule();

 	/* forward：是否要进行目录的递归遍历的操作 */
	int Scan(const char *dir, bool forward = false);
	/* 释放m_pList链表 */
	void Clear();
	void FileFirst();	
	int FileNext(char *dir, char *fname);
	int GetFileCnt() {return m_nFileCnt;}
	const char* GetDir() {return m_strDir;}

	enum {
		SEARCH_EOF = 1,
		ORG_LIST_CNT = 50,
	};
private:	
	virtual int RuleCheck(const char *dir, const char *fname);
	int ScanDir(const char *dir, bool forward = false);
	int Add2List(const char *dir, const char *fname);
	int ExpandList();
	void ReleaseList();

	int m_nTotalCnt;	// 列表的空间大小	
	int m_nFileCnt;		// 目前占用的列表空间	
	int m_nIndex;		// 当前枚举的位置	
	char m_strDir[256];
	FILE_INFO *m_pList;	// 列表指针
};

#endif
