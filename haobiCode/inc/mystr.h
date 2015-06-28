#ifndef _MY_STR_H
#define _MY_STR_H


//---------------------------------------------------
//				�ո����⴦��
//---------------------------------------------------
char* ltrim(char *p);
char* rtrim(char *p);
char* trim(char *p);

/* win��ȥ��\r\n��linux��ȥ��\n */
char* chomp(char *p);

/*
����˵����

���룺
strText ����Ҫ������ַ���
strSrc  ����Ҫ�滻���ַ���
strDst  ���滻����ַ���

�����
�����滻�õ��ַ�����ע�ⷵ�ص��ַ���ָ����ڴ�ռ���Ҫ�û������ͷ�
*/
char* strreplace(const char *strText, const char *strSrc, const char *strDst);

#define is_uc_letter(ch) ('A' <= (ch) && (ch) <= 'Z')
#define is_lc_letter(ch) ('a' <= (ch) && (ch) <= 'z')
#define is_letter(ch) (is_uc_letter(ch) || is_lc_letter(ch))
#define is_digit(ch) ('0' <= (ch) && (ch) <= '9')

bool IsStrDigit(const char *strText);
bool IsStrLetter(const char *strText);

#endif