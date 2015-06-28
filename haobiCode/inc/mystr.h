#ifndef _MY_STR_H
#define _MY_STR_H


//---------------------------------------------------
//				空格问题处理
//---------------------------------------------------
char* ltrim(char *p);
char* rtrim(char *p);
char* trim(char *p);

/* win：去掉\r\n，linux：去掉\n */
char* chomp(char *p);

/*
参数说明：

输入：
strText ：需要处理的字符串
strSrc  ：需要替换的字符串
strDst  ：替换后的字符串

输出：
返回替换好的字符串，注意返回的字符串指针的内存空间需要用户自行释放
*/
char* strreplace(const char *strText, const char *strSrc, const char *strDst);

#define is_uc_letter(ch) ('A' <= (ch) && (ch) <= 'Z')
#define is_lc_letter(ch) ('a' <= (ch) && (ch) <= 'z')
#define is_letter(ch) (is_uc_letter(ch) || is_lc_letter(ch))
#define is_digit(ch) ('0' <= (ch) && (ch) <= '9')

bool IsStrDigit(const char *strText);
bool IsStrLetter(const char *strText);

#endif