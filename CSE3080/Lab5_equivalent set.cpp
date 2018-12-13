#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 24
//#define IS_FULL (ptr) (!(ptr))
#define FALSE 0
#define TRUE 1
typedef struct node *node_pointer;
typedef struct node {
	int data;
	node_pointer link;
};

void push (int data, node_pointer *top)
{
	node_pointer t;
	t = (node_pointer)malloc(sizeof(node));
	t->data = data;
	t->link = *top;
	*top = t;
}

int pop (node_pointer *top)
{
	int re;
	node_pointer t;
	t = (node_pointer)malloc(sizeof(node));
	t = (*top);
	re = t->data;
	*top =t->link;
	free(t);

	return re;
}
void main(void)
{
	FILE *fp;
	short int out[MAX_SIZE];
	node_pointer seq[MAX_SIZE];
	node_pointer x, y, top;
	int i, j, n;
	
	fp = fopen("A.txt", "r");
	//printf("Enter the size (<= %d) ", MAX_SIZE);
	fscanf(fp,"%d", &n);
	for(i = 0; i < n; i++) {
		/* initialize seq and out */
		out[i] = TRUE; seq[i] = NULL;
	}
//	printf("Enter a pair of numbers (-1 –1 to quit) : ");
//	scanf("%d%d", &i, &j);

	while(1)
	{
		fscanf(fp,"%d %d", &i, &j);
		if(i == -1 || j == -1)
			break;

		x = (node_pointer)malloc(sizeof(node));
		if(x==NULL) {
			fprintf(stderr, "The memory is full\n");
			exit(1);
		}
		x->data = j; x->link = seq[i]; seq[i] = x;
		x = (node_pointer)malloc(sizeof(node));
		if(x==NULL) {
			fprintf(stderr, "The memory is full\n");
			exit(1);
		}
		x->data = i; x->link = seq[j]; seq[j] = x;
	}
	

/*	while(i >=0) {
		x = (node_pointer)malloc(sizeof(node));
		if(x==NULL) {
			fprintf(stderr, "The memory is full\n");
			exit(1);
		}
		x->data = j; x->link = seq[i]; seq[i] = x;
		x = (node_pointer)malloc(sizeof(node));
		if(x==NULL) {
			fprintf(stderr, "The memory is full\n");
			exit(1);
		}
		x->data = i; x->link = seq[j]; seq[j] = x;
		printf("Enter a pair of numbers (-1 –1 to quit) : ");
		scanf("%d%d", &i, &j);
	}*/
		/* Phase 2 : output the equivalence classes */
	for(i = 0; i < n; i++) {
		if(out[i]) {
			printf("\nNew Class : %5d", i);
			out[i] =FALSE; 
			x = seq[i]; top = NULL; 
			for(; ; ) { 
				while(x) { 
					j = x->data;
					if(out[j]) {
						printf("%5d", j); out[j] = FALSE;
						push(j, &top);
						//y = x->link; x->link = top; top = x; x = y;
					}
					else x = x->link;
				}
				if(!top) break;
				x = seq[pop(&top)];
				//x = seq[top->data]; top = top->link; 
			}
		}
	}

	fclose(fp);
	for(int i=0; i<100000; i++)
		for(int j=0; j<1000000; j++)
			for(int k=0; k<10000000; k++);
}