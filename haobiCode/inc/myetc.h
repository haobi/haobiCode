#ifndef _MY_ETC_H
#define _MY_ETC_H

#include <stdio.h>
#include <stdlib.h>
#include <myDef.h>

#include <map>
#include <string>

using namespace std;

/*
 * 注意事项：
 * 1. seg不能超过30个字符
 * 2. item不能超过30个字符
 * 3. 信息部分不要超过80个字符，若超过会自动截断
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
		关于返回值：
		0：正常获取配置值
		1：无法获取配置值或是使用默认值
		-1：返回出错
	*/
	int GetValue(const char *seg,const char *item,char *buf);
	/* 各种类型的转换 */
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