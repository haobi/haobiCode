#include "myetc.h"
#include "mystr.h"
#include "myfdcntl.h"

MyEtc::MyEtc() {

}

MyEtc::~MyEtc() {

}

int MyEtc::Init(const char *filepath) {

	int rs;

	_CHECK_RS(filepath,NULL,-1);

	rs = IsFile(filepath);
	_CHECK_RS(rs,-1,-1);

	if (1 == rs)
		return -1;

	m_EtcInfo.clear();

	rs = AnalyseText(filepath);
	_CHECK_RS(rs,-1,-1);
	
	return 0;
}

int MyEtc::GetValue(const char *seg,const char *item,char *buf) {

	int len;
	const char *p;
	char tbuf[128] = {0};

	if (!seg || !item || !buf)
		return -1;

	snprintf(tbuf,sizeof(tbuf),"%s|%s",seg,item);

	p = m_EtcInfo[tbuf].c_str();
	len = strlen(p);
	memcpy(buf,p,len);

	buf[len] = 0;

	if (0 == len)
		return 1;
	return 0;
}

int MyEtc::GetValue2Int(int &val, const char *seg, const char *item, int def) {
	
	int nRs;
	char strVal[128] = {0};
	
	nRs = GetValue(seg, item, strVal);
	_CHECK_RS(nRs, -1, -1);
	
	if (0 == nRs)
		val = atoi(strVal);
	else
		val = def;
	return nRs;
}

int MyEtc::GetValue2Long(long &val, const char *seg, const char *item, long def) {
	
	int nRs;
	char strVal[128] = {0};
	
	nRs = GetValue(seg, item, strVal);
	_CHECK_RS(nRs, -1, -1);
	
	if (0 == nRs)
		val = atol(strVal);
	else
		val = def;
	return nRs;
}

int MyEtc::GetValue2Double(double &val, const char *seg, const char *item, double def) {

	int nRs;
	char strVal[128] = {0};
	
	nRs = GetValue(seg, item, strVal);
	_CHECK_RS(nRs, -1, -1);
	
	if (0 == nRs)
		val = atof(strVal);
	else
		val = def;
	return nRs;
}

int MyEtc::GetValue2Bool(bool &val, const char *seg, const char *item, bool def) {
	
	int nRs;
	char strVal[128] = {0};
	
	nRs = GetValue(seg, item, strVal);
	_CHECK_RS(nRs, -1, -1);
	
	if (0 == strcmp("false", strVal)) {
		val = false;
	}
	else {
		val = true;
	}
	return nRs;
}

int MyEtc::GetValue2Str(char *buf, const char *seg, const char *item, const char *def) {

	int nRs;
	
	nRs = GetValue(seg, item, buf);
	_CHECK_RS(nRs, -1, -1);
	
	if (1 == nRs)
		strcpy(buf, def);
	return nRs;
}

int MyEtc::AnalyseText(const char *filepath) {

	int rs;
	char buffer[1024] = {0};
	char seg[SEG_LEN + 1] = {0};
	char item[ITEM_LEN + 1] = {0};
	char value[VALUE_LEN + 1] = {0};
	FILE *fp;

	fp = fopen(filepath,"r");
	_CHECK_RS(fp,NULL,-1);

	while (fgets(buffer,sizeof(buffer),fp)) {
		/* 分析每一行文本，获取其seg或item */
		char *p = chomp(buffer);
		int len = strlen(p);
		int i = 0;

		while (0 != p[i]) {
			if (' ' == p[i] || '\t' == p[i]) {
				i++;
				continue;
			}
			if ('#' == p[i])
				break;
			/* 获取该seg */
			if ('[' == p[i]) {
				rs = GetSegName(p + i + 1,seg);
				if (!rs) {
					i = i + strlen(seg) + 1;
					break;
				}
			}
			/* 获取item */
			if (strlen(seg)) {
				rs = GetItemName(p + i,item,value);
				if (0 == rs) {
					char tbuf[128] = {0};
#ifdef SYSTEM_WIN
					_snprintf(tbuf,sizeof(tbuf),"%s|%s",seg,item);
#else
					snprintf(tbuf,sizeof(tbuf),"%s|%s",seg,item);
#endif
					m_EtcInfo[tbuf] = value;
				}
				break;
			}
		}
	}
	fclose(fp);
	return 0;
}

int MyEtc::GetSegName(const char *str,char *buf) {

	if (!str || !buf)
		return -1;

	int len,pos,flag;
	pos = 0;
	flag = 0;
	len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (']' == str[i]) {
			flag = 1;
			break;
		}
		if (pos >= SEG_LEN)
			break ;
		buf[pos++] = str[i];
	}
	if (flag && pos) {
		buf[pos] = 0;
		trim(buf);
		return 0;
	}
	buf[0] = 0;
	return 1;
}

int MyEtc::GetItemName(const char *str,char *key,char *value) {

	char *p;
	int pos, nFlag;

	if (!str || !key || !value)
		return -1;
	if ('=' == str[0] || '#' == str[0])
		return 1;

	nFlag = ITEM_MARK;
	pos = 0;
	p = key;

	for (int i = 0; i < strlen(str); i++) {
		if ('#' == str[i])	
			break;
		if ('\t' == str[i])
			continue;
		if ('=' == str[i]) {
			/* key已经存放完毕 */
			p[pos] = 0;
			p = value;
			pos = 0;
			nFlag = VALUE_MARK;
			continue;
		}
		if (ITEM_MARK == nFlag && pos >= ITEM_LEN)
			continue ;	
		if (VALUE_MARK == nFlag && pos >= VALUE_LEN)
			continue ;
		p[pos++] = str[i];
	}
	p[pos] = 0;
	if (strlen(key) && strlen(value)) {
		trim(key);
		trim(value);
		return 0;
	}
	key[0] = value[0] = 0;
	return 1;
}

void MyEtc::ShowItemInfo() {

	map<string, string>::iterator it;
	for (it = m_EtcInfo.begin(); it != m_EtcInfo.end(); it++) {
		printf("ITEM[%s] --> VALUE[%s] .\n", 
			it->first.c_str(), it->second.c_str());
	}
}