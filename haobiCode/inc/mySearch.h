#ifndef _MY_SEARCH_H
#define _MY_SEARCH_H

#include "myDef.h"

typedef int (*cmp_fun)(const void *, const void *);

// 通用二分算法
/*
 * 必要条件：数组一定要顺序排列的，不然查找算法失效
 * 参数：
 * 1. array : 要查找数组的指针
 * 2. size  : 每个元素的大小
 * 3. cnt   : 数组元素的数量
 * 4. key   : 要查找的key
 * 5. fun   : 查找比对函数
 * 
 * fun函数定义，例子如下：
 * int fdi_cmp(const void *p0, const void *p1) {
 *		... ...
 *		if (p0 > p1)
 *			return 1;
 *		else if (p0 > p1)
 *			return -1;
 *		return 0;
 * }
 * 注意：返回1为从大到小排序，否则反之。
 */
int BinarySearch(void *array, int size, int cnt,const void *key, cmp_fun fun);

#endif
