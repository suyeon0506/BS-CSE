#include<stdio.h>
#include<stdlib.h>
#define MAX_SIZE 50

typedef enum { head, entry } tagfield;
typedef struct matrix_node *matrix_pointer;
typedef struct {
	int row;
	int col;
	int value;
} entry_node;
typedef struct matrix_node {
	matrix_pointer down;
	matrix_pointer right;
	tagfield tag;
	union {
		matrix_pointer next;
		entry_node entry;
	} u;
} matrix_node;

matrix_pointer mread(FILE*); // file에서 matrix 자료를 읽어 리스트에 저장한다.
void mwrite(matrix_pointer);  // 행렬을 적절한 포맷 (2차원 행렬형태)으로 출력한다.
void merase(matrix_pointer*); // NULL parameter인 경우 적절히 처리.

void main()
{
	matrix_pointer a, b;
	FILE *fp;

	fp = fopen("A.txt", "r");
	a = mread(fp);
	fclose(fp);
	fp = fopen("B.txt", "r");
	b = mread(fp);
	fclose(fp);
	mwrite(a);
	mwrite(b);
	merase(&a);
	merase(&b);
	mwrite(a);
	mwrite(b);
}

matrix_pointer mread(FILE *fp)
{
	matrix_pointer hdnode[MAX_SIZE];
	int num_rows, num_cols, num_heads, i, j;
	int value, current_row;
	matrix_pointer temp, last, node;

	fscanf(fp, "%d %d", &num_rows, &num_cols);
	num_heads = (num_cols > num_rows) ? num_cols : num_rows;
	node = (matrix_pointer)malloc(sizeof(matrix_node));
	node->tag = entry; node->down = NULL; node->right = NULL;
	node->u.entry.row = num_rows;
	node->u.entry.col = num_cols;
	if (!num_heads)
		node->right = node;
	else
	{
		for (i = 0; i < num_heads; i++)
		{
			temp = (matrix_pointer)malloc(sizeof(matrix_node));
			hdnode[i] = temp; hdnode[i]->tag = head;
			hdnode[i]->right = temp; hdnode[i]->u.next = temp;
		}
		current_row = 0; last = hdnode[0];
		for (i = 0; i < num_rows; i++)
		{
			for (j = 0; j < num_cols; j++)
			{
				fscanf(fp, "%d", &value);
				if (i > current_row)
				{
					last->right = hdnode[current_row];
					current_row = i;
					last = hdnode[i];
				}
				if (value)
				{
					temp = (matrix_pointer)malloc(sizeof(matrix_node));
					temp->tag = entry;
					temp->u.entry.row = i; temp->u.entry.col = j;
					temp->u.entry.value = value;
					last->right = temp; last = temp;
					hdnode[j]->u.next->down = temp;
					hdnode[j]->u.next = temp;
				}
			}
			last->right = hdnode[current_row];
		}

		for (i = 0; i < num_cols; i++)
			hdnode[i]->u.next->down = hdnode[i];
		for (i = 0; i < num_heads - 1; i++)
			hdnode[i]->u.next = hdnode[i + 1];
		hdnode[num_heads - 1]->u.next = node;
		node->right = hdnode[0];
	}

	return node;
}

void mwrite(matrix_pointer node)
{
	int i;
	matrix_pointer temp, head;

	if (node == NULL)
		return;

	head = node->right;
	printf("row\tcol\tval\n");
	for (i = 0; i < node->u.entry.row; i++)
	{
		for (temp = head->right; temp != head; temp = temp->right)
			printf("%d\t%d\t%d\n", temp->u.entry.row, temp->u.entry.col, temp->u.entry.value);
		head = head->u.next;
	}
}

void merase(matrix_pointer *node)
{
	int i;
	matrix_pointer x, y, head;

	if (*node == NULL)
		return;

	head = (*node)->right;
	for (i = 0; i < (*node)->u.entry.row; i++)
	{
		y = head->right;
		while (y != head)
		{
			x = y; y = y->right;
			free(x);
		}
		x = head; head = head->u.next; free(x);
	}
	y = head;
	while (y != *node)
	{
		x = y; y = y->u.next;
		free(x);
	}
	free(*node);
	*node = NULL;
}