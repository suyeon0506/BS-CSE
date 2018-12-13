#include <stdio.h>
#include <string.h>
#include "my_types.h"

int INSERTION_SORT(ELEMENT data[], int left, int right) {
	// must return 1 if the function finishes normally or return 0 otherwise
	//	return 0;
	int len = right - left + 1;
	int i, j;
	ELEMENT tmp;

	for (i = 1; i < len; i++)
	{
		//tmp = data[i];
		memcpy(&tmp, data + i, sizeof(ELEMENT));
		j = i;
		while ((j > 0) && (tmp.key < ELEMENT_KEY(data+(j-1))))
		{
			memcpy(data+j, data+(j - 1), sizeof(ELEMENT));
			//data[j] = data[j - 1];
			//ELEMENT_KEY(data+j) = ELEMENT_KEY(data+(j - 1));
			j = j - 1;
		}

		//ELEMENT_KEY(data+j) = tmp;
		//data[j] = tmp;
		memcpy(data + j, &tmp, sizeof(ELEMENT));
	}

	return 1;
}