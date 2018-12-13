#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_types.h"
void SWAP2(ELEMENT *a, ELEMENT *b, ELEMENT *temp)
{
	memcpy(temp, a, sizeof(ELEMENT));
	memcpy(a, b, sizeof(ELEMENT));
	memcpy(b, temp, sizeof(ELEMENT));
}

void adjust(ELEMENT data[], int root, int n)
{
	int child, rootkey;
	ELEMENT temp;
	memcpy(&temp, data + root, sizeof(ELEMENT));
	//temp = data[root];
	rootkey = data[root].key;
	child = 2 * root;
	while (child <= n)
	{
		if ((child < n) && (data[child].key < data[child + 1].key))
			child = child + 1;
		if (rootkey > data[child].key)
			break;
		else
		{
			//data[child / 2] = data[child];
			memcpy(data + (child/2), data + child, sizeof(ELEMENT));
			child = child * 2;
		}
	}

	//data[child / 2] = temp;
	memcpy(data+(child/2), &temp, sizeof(ELEMENT));

}

int HEAP_SORT(ELEMENT data[], int left, int right) {
	// must return 1 if the function finishes normally or return 0 otherwise
	//	return 0;
	int len = right - left + 1;
	int i;
	
	ELEMENT *temp;
	temp = (ELEMENT*)malloc(sizeof(ELEMENT));

	for (i = (len / 2); i >= 0; i--)
		adjust(data, i, len);
	for (i = len-1 ; i > 0; i--)
	{
		SWAP2(data, data + (i+1), temp);
		adjust(data, 0, i);
	}

	SWAP2(data, data + 1, temp);
	free(temp);
	return 1;
}