#include "myDef.h"
#include "myfdcntl.h"
#include "myFileSearch.h"
#ifdef SYSTEM_WIN
	#include <io.h>
#else
	#include <dirent.h>
#endif

TFileSearchRule::TFileSearchRule() {
	m_pList = NULL;
	m_nTotalCnt = 0;
	m_nFileCnt = 0;
	m_nIndex = 0;
	memset(m_strDir, 0, sizeof(m_strDir));
}

TFileSearchRule::~TFileSearchRule() {
	ReleaseList();
}

int TFileSearchRule::Scan(const char *dir, bool forward) {

	_CHECK_RS(dir, NULL, -1);

	ReleaseList();

	m_nFileCnt = m_nIndex = 0;
	strncpy(m_strDir, dir, sizeof(m_strDir));

	return ScanDir(dir, forward);
}

int TFileSearchRule::ScanDir(const char *dir, bool forward) {

	int res, rs;
	char strPath[256];
	const char *strFileName;
#ifdef SYSTEM_WIN
	long iFind;
	_finddata_t fileinfo;
#else
	DIR *dp;
	struct dirent *dirp;
#endif

	if (NULL == dir)
		return -1;
#ifdef SYSTEM_WIN
	memset(&fileinfo, 0, sizeof(fileinfo));
#endif
	snprintf(strPath, sizeof(strPath), "%s/*", dir);

#ifdef SYSTEM_WIN
	iFind = _findfirst(strPath, &fileinfo);
	_CHECK_RS(iFind, -1, -1);
#else
	dp = opendir(strPath);
	_CHECK_RS(dp, NULL, -1);
#endif

#ifdef SYSTEM_WIN
	while (0 == _findnext(iFind, &fileinfo)) {
		if ('.' == fileinfo.name[0])
			continue ;
		strFileName = fileinfo.name;
#else
	while (NULL != (dirp = readdir(dp))) {
		if ('.' == dirp->d_name[0])
			continue ;
		strFileName = dirp->d_name;
#endif

		snprintf(strPath, sizeof(strPath), "%s/%s", dir, strFileName);
		if (0 == IsDir(strPath) && true == forward) {
			rs = ScanDir(strPath, forward);
			_CHECK_GOTO(rs, -1, out);
			continue ;
		}
		if (0 == IsFile(strPath)) {
			rs = RuleCheck(dir, strFileName);
			_CHECK_GOTO(rs, -1, out);
			if (0 == rs) {
				rs = Add2List(dir, strFileName);
				_CHECK_GOTO(rs, -1, out);
			}
		}
	}
	res = 0;
out:
#ifdef SYSTEM_WIN
	_findclose(iFind);
#else
	closedir(dp);
#endif
	if (-1 == res)
		m_nFileCnt = 0;
	return res;
}

void TFileSearchRule::Clear() {
	ReleaseList();
}

void TFileSearchRule::FileFirst() {
	m_nIndex = 0;
}

int TFileSearchRule::FileNext(char *dir, char *fname) {

	if (NULL == dir || NULL == fname)
		return -1;

	if (m_nIndex >= m_nFileCnt)
		return SEARCH_EOF;

	strcpy(dir, m_pList[m_nIndex].strPath);	
	strcpy(fname, m_pList[m_nIndex].strFileName);	
	m_nIndex++;
	return 0;
}

int TFileSearchRule::RuleCheck(const char *dir, const char *fname) {
	return 0;
}

int TFileSearchRule::Add2List(const char *dir, const char *fname) {

	int rs;
	FILE_INFO *pFI;

	if (NULL == dir || NULL == fname)
		return -1;
	if (m_nTotalCnt == m_nFileCnt) {
		rs = ExpandList();
		_CHECK_RS(rs, -1, -1);
	}

	pFI = &m_pList[m_nFileCnt];
	strncpy(pFI->strPath, dir, sizeof(pFI->strPath));
	strncpy(pFI->strFileName, fname, sizeof(pFI->strFileName));

	m_nFileCnt++;
	return 0;
}

int TFileSearchRule::ExpandList() {

	int nSize;
	
	if (m_nTotalCnt > m_nFileCnt)
		return 0;

	if (0 == m_nTotalCnt) {
		m_nTotalCnt = ORG_LIST_CNT/*s_nOrgCnt*/;
		m_pList = (FILE_INFO *)calloc(sizeof(FILE_INFO) * m_nTotalCnt, 1);
		_CHECK_RS(m_pList, NULL, -1);
		return 0;
	}

	nSize = sizeof(FILE_INFO) * m_nTotalCnt * 2;
	m_pList = (FILE_INFO *)realloc(m_pList, nSize);
	_CHECK_RS(m_pList, NULL, -1);

	m_nTotalCnt *= 2;
	return 0;
}

void TFileSearchRule::ReleaseList() {

	m_nTotalCnt = 0;
	m_nFileCnt = 0;
	memset(m_strDir, 0, sizeof(m_strDir));

	if (NULL == m_pList)
		return ;
	FREE_PT(m_pList);
}
