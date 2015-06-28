#ifndef _MY_ETC_H
#define _MY_ETC_H

#include <stdio.h>
#include <stdlib.h>
#include <myDef.h>

#include <map>
#include <string>

using namespace std;

/*
 * ע�����
 * 1. seg���ܳ���30���ַ�
 * 2. item���ܳ���30���ַ�
 * 3. ��Ϣ���ֲ�Ҫ����80���ַ������������Զ��ض�
 * */
#define SEG_LEN 30
#define ITEM_LEN 30
#define VALUE_LEN 80

class MyEtc {

public:
	MyEtc();
	~MyEtc();

	int Init(const char *filepath);
	/*
		���ڷ���ֵ��
		0��������ȡ����ֵ
		1���޷���ȡ����ֵ����ʹ��Ĭ��ֵ
		-1�����س���
	*/
	int GetValue(const char *seg,const char *item,char *buf);
	/* �������͵�ת�� */
	int GetValue2Int(int &val, const char *seg, const char *item, int def);
	int GetValue2Long(long &val, const char *seg, const char *item, long def);
	int GetValue2Double(double &val, const char *seg, const char *item, double def);
	int GetValue2Bool(bool &val, const char *seg, const char *item, bool def);
	int GetValue2Str(char *buf, const char *seg, const char *item, const char *def);

	void ShowItemInfo();
private:
	int AnalyseText(const char *filepath);
	int GetSegName(const char *str,char *buf);
	int GetItemName(const char *str,char *key,char *value);

	enum {
		ITEM_MARK = 0,
		VALUE_MARK = 1,
	};
private:
	map<string,string> m_EtcInfo;
};

#endif