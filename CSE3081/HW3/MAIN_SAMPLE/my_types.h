#pragma once
//
// DO NOT MODIFY THIS FILE!!!
//

typedef struct {
	unsigned int key; float score; double other; char name[16];
} ELEMENT;
#define ELEMENT_KEY(x) ((x)->key)

// Each function must return 0 if it has finished normally or a nonzero value otherwise.
int SELECTION_REC(ELEMENT data[], int f, int b, int k, int *index);

int INSERTION_SORT(ELEMENT data[], int f, int b);
int HEAP_SORT(ELEMENT data[], int f, int b);
int QSORT(ELEMENT data[], int f, int b);
int QUICK_SORT(ELEMENT data[], int f, int b);
int QUICK_SORT_OPT(ELEMENT data[], int f, int b);

//
// DO NOT MODIFY THIS FILE!!!
//