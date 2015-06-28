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

 	/* forward���Ƿ�Ҫ����Ŀ¼�ĵݹ�����Ĳ��� */
	int Scan(const char *dir, bool forward = false);
	/* �ͷ�m_pList���� */
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

	int m_nTotalCnt;	// �б�Ŀռ��С	
	int m_nFileCnt;		// Ŀǰռ�õ��б�ռ�	
	int m_nIndex;		// ��ǰö�ٵ�λ��	
	char m_strDir[256];
	FILE_INFO *m_pList;	// �б�ָ��
};

#endif
