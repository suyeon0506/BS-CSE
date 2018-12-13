#include <stdio.h>
#include <stdlib.h>

#define MAX_TERMS 100
typedef struct treeNode* treePtr;
typedef struct treeNode {
    treePtr lchild;
    int data;
    treePtr rchild;
}treeNode;

void inorder (treePtr);
void preorder(treePtr);
void insert(treePtr*, int);
treePtr modifiedSearch(treePtr, int);

void main()
{
    int i,n,A[MAX_TERMS];
    treePtr tree=NULL;
    FILE *fp;
    fp=fopen("BST_input.txt","r");
    fscanf(fp,"%d",&n);
    /*for(i=0; i<n; i++)
    {
	fscanf(fp,"%d",&A[i]);
    }
*/
    i = 0; 
    while(!feof(fp)){ 
	fscanf(fp,"%d",&A[i]);
	i++;
    }

        
    for(i=0; i<10; i++)
	insert(&tree,A[i]);
    
	printf("In-order: ");
    inorder(tree);
    printf("\n");
    
	printf("Pre-order: ");
    preorder(tree);
    printf("\n");
   
    
}

void inorder(treePtr ptr)
{
    if(ptr)
    {
	inorder(ptr->lchild);
	printf("%d ",ptr->data);
	inorder(ptr->rchild);
    }
}

void preorder (treePtr ptr)
{
    if(ptr)
    {
	printf("%d ",ptr->data);
	preorder(ptr->lchild);
	preorder(ptr->rchild);
    }
}

void insert(treePtr *tree,int key)
{
    treePtr ptr,temp;
    temp=modifiedSearch(*tree,key);
    if(temp || !(*tree))
    {
	ptr = (treePtr)malloc(sizeof(treeNode));
	ptr->data = key;
	ptr->lchild = NULL;
	ptr->rchild = NULL;

	if(*tree)
	{
	    if(key < temp->data)
		temp->lchild=ptr;
	    else
		temp->rchild=ptr;
	}
	else
	    *tree=ptr;
    }
}

treePtr modifiedSearch(treePtr tree,int key)
{
    treePtr tmp;

    if(!tree)
	return NULL;

//    tmp = tree;
    while(tree)
    {
	if(key == tree->data)
	    return NULL;
	else if(key< tree->data)
	{
	    if(tree->lchild==NULL)
		return tree;
	    else
		tree= tree->lchild;
	}
	else
	{
	    if(tree->rchild==NULL)
		return tree;
	    else
		tree=tree->rchild;
	}
    }

    return NULL;
}


















