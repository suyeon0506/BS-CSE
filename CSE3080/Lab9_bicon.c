#include <stdio.h>
#include <stdlib.h>
#define MAX_VERTICES 30
#define MIN2(x,y) ((x)<(y) ? (x) : (y))
short int dfn[MAX_VERTICES];
short int low[MAX_VERTICES];

typedef struct node *node_pointer;
typedef struct node{
    int vertex, end_vertex;
    node_pointer link;
}node;

struct node** graph;
struct node* top;

int num;
void init();
void bicon(int,int);

void add_to_graph(struct node* graph, int v)
{
    node_pointer add,ptr;
    add=(struct node*)malloc(sizeof(struct node));

    add->link=NULL;
    add->vertex=v;
    ptr = graph;
    if (ptr==NULL)
	ptr = add;
    else{
        while(1)
        {
		if(ptr->link ==NULL)
	    	    break;
        	ptr=ptr->link;
	}	
	ptr->link=add;
    }
}
void push (struct node* top, int u, int v)
{
    node_pointer pt1,add1;
    pt1=top;
    
    add1=(struct node*)malloc(sizeof(struct node));
    add1->link=NULL;
    add1->vertex=u;
    add1->end_vertex=v;

    if(pt1->link !=NULL)
    {
	add1->link=pt1->link;
    }

    pt1->link=add1;
}

void pop(struct node* top, int *x, int*y)
{
    node_pointer pt1;
    pt1=top;

    if(pt1->link != NULL)
    {
	*x=pt1->link->vertex;
	*y=pt1->link->end_vertex;
	pt1->link=pt1->link->link;
    }
}

void main()
{
    FILE *fp=fopen("input.txt","r");
    char retErr,ch;
    int i,flag=0,key,root=0;

    fscanf(fp,"%d",&num);

    graph=(struct node**)malloc(sizeof(struct node*)*num);
    top=(struct node*)malloc(sizeof(struct node));
    top->link=NULL;

    for(i=0; i<num; i++)
    {	
	graph[i] = (node_pointer)malloc(sizeof(struct node));
	graph[i]->link=NULL;
	graph[i]->vertex = i;
    }
    while(1)
    {
		retErr=fscanf(fp,"%c",&ch);
		if(retErr==EOF)
			break;
		if(ch=='\n')
			flag=1;
		if(ch>='0' && ch<='9')
		{
			key=ch-'0';
			if(flag==0)
			{
				add_to_graph(graph[root],key);
			}
			else if(flag==1)
			{
				root=key;
				flag=0;
			}
		}
    }
    init();
    bicon(3,-1);

}

void init()
{
    int i;
    for(i=0; i<num; i++)
    {
	dfn[i]=low[i]=-1;
    }
    num=0;
}

void bicon (int u, int v)
{
    node_pointer ptr;
    int w,x,y;
    dfn[u]=low[u]=num++;
    for(ptr=graph[u]; ptr; ptr=ptr->link)
    {
	w=ptr->vertex;
	if(v!= w && dfn[w]<dfn[u])
	    push(&top,u,w);;
	if(dfn[w]<0)
	{
	    bicon(w,u);
	    low[u]=MIN2(low[u],low[w]);
	    if(low[w]>=dfn[u])
	    {
	//	printf("%d %d\n",u,w);
		printf("New biconnected componnent: ");
		do{
		    pop(&top,&x,&y);
		    printf("  <%d,%d>",x,y);
		}while (!((x==u) && (y==w)));
		printf("\n");
	    }
	}
	else if(w!=v) low[u] = MIN2(low[u],dfn[w]);
    }
}



