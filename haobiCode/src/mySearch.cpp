#include "mySearch.h"

int BinarySearch(void *array, int size, int cnt,const void *key, cmp_fun fun)
{
	if (array == NULL || key == NULL || cnt <= 0)
		return -1; 

	int low = 0, high = cnt - 1, mid;
	int rs; 
	char *pt;
	while (low <= high)
	{   
		mid = (low + high) / 2;
		pt = (char *)array + size * mid;
		rs = fun(key, pt);
		if (rs == 0)
			return mid;
		else if (rs < 0)
			high = mid - 1;
		else
			low = mid + 1;
	}   
	return -1; 
}
