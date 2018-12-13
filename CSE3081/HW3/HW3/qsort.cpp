#include "my_types.h"
#include <stdlib.h>

int keys_compare(const void *a, const void *b) {
	ELEMENT *rec_a, *rec_b;
	rec_a = (ELEMENT*)a;
	rec_b = (ELEMENT*)b;
	// do it yourself ;; 오름차순으로 정렬하기 위해 만드는 함수
	if (ELEMENT_KEY(rec_a) > ELEMENT_KEY(rec_b))
		return 1;
	else if (ELEMENT_KEY(rec_a) < ELEMENT_KEY(rec_b))
		return -1;
	else
		return 0;
}

int QSORT(ELEMENT data[], int left, int right) {
	// must return 1 if the function finishes normally or return 0 otherwise

	qsort(data + left, right-left+1, sizeof(ELEMENT), keys_compare);
	return 1;
}