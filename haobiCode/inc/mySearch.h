#ifndef _MY_SEARCH_H
#define _MY_SEARCH_H

#include "myDef.h"

typedef int (*cmp_fun)(const void *, const void *);

// ͨ�ö����㷨
/*
 * ��Ҫ����������һ��Ҫ˳�����еģ���Ȼ�����㷨ʧЧ
 * ������
 * 1. array : Ҫ���������ָ��
 * 2. size  : ÿ��Ԫ�صĴ�С
 * 3. cnt   : ����Ԫ�ص�����
 * 4. key   : Ҫ���ҵ�key
 * 5. fun   : ���ұȶԺ���
 * 
 * fun�������壬�������£�
 * int fdi_cmp(const void *p0, const void *p1) {
 *		... ...
 *		if (p0 > p1)
 *			return 1;
 *		else if (p0 > p1)
 *			return -1;
 *		return 0;
 * }
 * ע�⣺����1Ϊ�Ӵ�С���򣬷���֮��
 */
int BinarySearch(void *array, int size, int cnt,const void *key, cmp_fun fun);

#endif
