#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_types.h"

void SWAP(ELEMENT *a, ELEMENT *b)
{
	ELEMENT *temp;
	temp = (ELEMENT*)malloc(sizeof(ELEMENT));
	memcpy(temp, a, sizeof(ELEMENT));
	memcpy(a, b, sizeof(ELEMENT));
	memcpy(b, temp, sizeof(ELEMENT));

	free(temp);
}

int partition(ELEMENT data[], int left, int right)
{
	int pivot = left;
	int i;

	for (i = left; i < right; i++)
	{
		if (ELEMENT_KEY(data + i) < ELEMENT_KEY(data + right))
		{
			SWAP((data+i), (data+pivot));
			pivot = pivot + 1;
		}
	}

	SWAP((data+right), (data+pivot));

	return pivot;
}

int QUICK_SORT(ELEMENT data[], int left, int right) {
	// must return 1 if the function finishes normally or return 0 otherwise
	//	return 0;
	int pivot;

	if (right - left > 0)
	{
		pivot = partition(data, left, right);

		QUICK_SORT(data, left, pivot - 1);
		QUICK_SORT(data, pivot + 1, right);
	}

	return 1;
}