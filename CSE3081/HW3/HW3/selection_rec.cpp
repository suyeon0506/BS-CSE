#include <stdio.h>
#include <stdlib.h> //malloc이용을 위한 추가
#include <string.h> //memcpy이용을 위한 추가
#include "my_types.h"

int SELECTION_REC(ELEMENT data[], int left, int right, int k, int *index) {
	// must return 1 if the function finishes normally or return 0 otherwise
	//	return 0;
	*index = 3;
	int len = right - left + 1;
	int finish = 1;
	int pivot = 0;

	ELEMENT *buffer;
	buffer = (ELEMENT*)malloc(sizeof(ELEMENT)*len);
	memcpy(buffer + left, data + left, sizeof(ELEMENT)); //데이터를 buffer에 복사해놓음

	ELEMENT *S1;
	S1 = (ELEMENT*)malloc(sizeof(ELEMENT));

	ELEMENT *S2;
	S2 = (ELEMENT*)malloc(sizeof(ELEMENT));

	ELEMENT temp[5];
	ELEMENT *store;
	store =(ELEMENT*)malloc(sizeof(ELEMENT)*((len / 5) + 1));
	int i, j, h, n;
	int store_index = 0, store_key, index1 = 0, index2 = 0;

	if (len < 50)
	{
		INSERTION_SORT(data, left, right);
		i = 0;
		while (ELEMENT_KEY(data + k - 1) != ELEMENT_KEY(buffer + i))
			i++;
		*index = i;
		return 1;
	}
	else
	{
		for (j = 0; j < len; j++)
		{
			for (h = 0; h < 5; h++)
			{
				if (j == len)
					break;
				temp[h] = data[j];
				j++;
			}
			INSERTION_SORT(temp, 0, 4);
			store_key = ELEMENT_KEY(temp + 2);
			ELEMENT_KEY(store+(store_index)) = store_key;
			store_index++;
			j--;
		}
		
		finish = SELECTION_REC(store, 0, (len/5),((len/5))/2 , index); //1반환
		
		pivot = ELEMENT_KEY(buffer + *index);
		for (n = 0; n < len; n++)
		{
			if (ELEMENT_KEY(buffer + n) < pivot)
			{
				S1[index1] = buffer[n];
				index1++;
			}
			else if (ELEMENT_KEY(buffer + n) > pivot)
			{
				S2[index2] = buffer[n];
				index2++;
			}
		}

		if (index1 >= k)
			finish = SELECTION_REC(S1, 0, index1 - 1, k, index);
		else if ((index1 + index2) >= k)
			return 1;
		else
			finish = SELECTION_REC(S2, 0, index2 - 1, k - index1 - 1, index);
	}

	free(buffer);
	free(S1);
	free(S2);
	free(store);

	return 1;
}
