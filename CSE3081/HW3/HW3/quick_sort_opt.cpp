#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_types.h"
///insertion ÀÌ¿כ///
#define less(A,B) (ELEMENT(A).key <ELEMENT(B).key)

void exch(ELEMENT *a, ELEMENT *b)
{
	ELEMENT *temp;
	temp = (ELEMENT*)malloc(sizeof(ELEMENT));
	memcpy(temp, a, sizeof(ELEMENT));
	memcpy(a, b, sizeof(ELEMENT));
	memcpy(b, temp, sizeof(ELEMENT));

	free(temp);
}

void compexch(ELEMENT *a, ELEMENT *b)
{
	if (less(*b, *a))
		exch(a, b);
}

int divide(ELEMENT data[], int left, int right)
{
	int i = left - 1;
	int j = right;
	ELEMENT temp;
	memcpy(&temp, data + right, sizeof(ELEMENT));
	for (;;)
	{
		while (less(data[++i], temp));
		while (less(temp, data[--j]))
		{
			if (j = left)
				break;
		}
		if (i >= j)
			break;
		exch(data+i, data+j);
	}

	exch(data+i, data+right);
	return i;
}

void quick_sort(ELEMENT data[], int left, int right)
{
	int i;
	int len = right - left + 1;
	if (right - left <= len)
		return;
	exch(data+((right + left) / 2), data+(right - 1));
	compexch(data+left, data+(right - 1));
	compexch(data+left, data+right);
	compexch(data+(right - 1), data+right);
	i = divide(data, left + 1, right - 1);
	quick_sort(data, left, i - 1);
	quick_sort(data, i + 1, right);
}
int QUICK_SORT_OPT(ELEMENT data[], int left, int right) {
	// must return 1 if the function finishes normally or return 0 otherwise
	//	return 0
	quick_sort(data, left, right);
	INSERTION_SORT(data, left, right);
	return 1;
}