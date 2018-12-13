#include<stdio.h>
#include<stdlib.h>

#define MAX_TERMS 100

typedef struct treeNode* treePtr;
typedef struct treeNode
{
	treePtr leftChild;
	int key;
	treePtr rightChild;
} treeNode;

void inorder(treePtr tree, int k1, int k2);
//void preorder(treePtr tree);
void insert(treePtr*tree, int key);
treePtr modifiedSearch(treePtr tree, int key);

void main()
{
	int i, n, A[MAX_TERMS],k1,k2;
//	FILE *fp;
	treePtr tree = NULL;
/*	fp = fopen("data.txt", "r");
	fscanf(fp, "%d", &n);
	for(i = 0; i < n; i++)
	{
		fscanf(fp, "%d", &A[i]);
	}
*/	
	scanf("%d",&n); //노드의 개수
	for(i=0; i<n; i++)
	    scanf("%d ",&A[i]);
	scanf("%d %d",&k1,&k2);
	for(i = 0; i < n; i++)
	{
		insert(&tree, A[i]);
	}
	inorder(tree,k1,k2);
	printf("\n");
//	preorder(tree);
//	fclose(fp);
}
/*void preorder(treePtr ptr)
{
	if(ptr)
	{
		printf("%-3d", ptr->data);
		preorder(ptr->leftChild);
		preorder(ptr->rightChild);
	}
	printf("\n");
}*/
void inorder(treePtr ptr,int k1,int k2)
{
	if(ptr)
	{
		inorder(ptr->leftChild,k1,k2);
		if(ptr->key >= k1 && ptr->key <= k2)
		    printf("%d ", ptr->key);
		inorder(ptr->rightChild,k1,k2);
	}
}
void insert(treePtr *tree, int key)
{
	treePtr temp, ptr;
	temp = modifiedSearch(*tree, key);
	if(temp || !(*tree))
	{
		ptr = (treePtr)malloc(sizeof(treeNode));
		/*num is not in the tree*/
		ptr->key = key;
		ptr->leftChild = ptr->rightChild = NULL;
		if(*tree)
		{
			if(key < temp->key) temp->leftChild = ptr;
			else temp->rightChild = ptr;
		}
		else *tree = ptr;
	}
}
treePtr modifiedSearch(treePtr tree, int key)
{
	if(tree == NULL) return NULL;
	while(tree)
	{
		if(key == tree->key) return NULL;
		else if(key < tree->key)
		{
			if(tree->leftChild == NULL) return tree;
			else tree = tree->leftChild;
		}
		else
		{
			if(tree->rightChild == NULL) return tree;
			else tree = tree->rightChild;
		}
	}
	return NULL;
}
