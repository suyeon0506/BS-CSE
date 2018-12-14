#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include "..\ExternDoc.h"
#include "..\ExternView.h"
#include "..\UI\Zoom.h"
#include "..\UI\MsgView.h"
#include "..\Graphics\DrawFunc.h"
#include "..\Example\Example.h"

#define ERROR_NUMBER -1

//function prototype

static void drawDirect(CDC *pDC);
static void drawBuffered();

//Start of user code
#include <float.h>

typedef struct maze* new_maze; 
typedef struct maze{
	int up,down,right,left;
}maze;
new_maze* MAZE;
char** arr;
int row,col;

typedef struct{
	int x,y;
}element;

element store;
element *stack;
element *queue;
element *history; //BFS에서 필요
int **visited;

int top = -1;
int is_dfs = 0;
int is_bfs = 0;
int do_free = 0;
int front = -1;
int rear =-1;
int n=0;

void push(int x, int y)
{
	top++;
	stack[top].x=x;
	stack[top].y=y;
}

element pop()
{
	if(top == -1)
		printf("Stack is empty!\n");
	else
		return stack[top--];
}

void addq (int x, int y)
{
	rear++;
	queue[rear].x=x;
	queue[rear].y=y;
}

element deleteq()
{
	if(front == rear)
		printf("Queue is empty!\n");
	else
		return queue[++front];
}

/*****************************************************************
* function	: bool readFile(const char* filename)
* argument	: cons char* filename - filename to be opened
* return	: true if success, otherwise flase
* remark	: After read data file, phisycal view must be set;
*			  otherwise drawing will not be executed correctly.
*			  The window will be invalidated after readFile()
*			  returns true.
******************************************************************/

bool readFile(const char* filename){
	
	//start of the user code
	FILE* fp;
	int i,j;
	char ch;
	int height,width;

	fp=fopen(filename,"r");
	if(fp==NULL)
		return false;
	row=0;
	col=1;

	while(1)
	{
		fscanf(fp,"%c",&ch);
		if(ch == ' ' || ch == '|' || ch == '\n')
			break; //예외사항 처리
		row++;
	}

	while(fscanf(fp,"%c",&ch)!=EOF)
	{
		if(ch=='\n' || ch == '\0')
			col++;
	}

	fclose(fp);

	fp=fopen(filename,"r");
	arr=(char**)malloc(sizeof(char*)*(col+1));
	for(i=0; i<col+1; i++)
		arr[i]=(char*)malloc(sizeof(char)*(row+1));

	for(i=0; i<col+1; i++)
	{
		for(j=0; j<row+1; j++)
		{
			fscanf(fp,"%c",&ch);
			arr[i][j]=ch;
			if(ch=='\n' || ch=='\0')
			{
				arr[i][j]='\0';
				break;
			}

		}
		if(ch=='\0')
			break;
	}
	row--;
	width=row/2;
	height=col/2;
	MAZE=(new_maze*)malloc(sizeof(maze*)*height);
	for(i=0; i<height; i++)
	{
		MAZE[i]=(maze*)malloc(sizeof(maze)*width);
		for(j=0; j<width; j++)
		{
			MAZE[i][j].down=0;
			MAZE[i][j].up=0;
			MAZE[i][j].right=0;
			MAZE[i][j].left=0;
		}
	}//메모리 할당 후 초기화
	for(i=0; i<col; i++)
	{
		if(i%2==1)
		{
			for(j=0; j<row; j++)
			{
				if(j%2==1)
				{
					if(arr[i-1][j]=='-')
						MAZE[i/2][j/2].up=1;
					if(arr[i+1][j]=='-')
						MAZE[i/2][j/2].down=1;
					if(arr[i][j-1]=='|')
						MAZE[i/2][j/2].left=1;
					if(arr[i][j+1]=='|')
						MAZE[i/2][j/2].right=1;
				}
			}
		}
	}
	setWindow(0,0,row/2, col/2, 1);
	fclose(fp);



	return true; //edit after finish this function
	//end of usercode
}

/******************************************************************
* function	: bool FreeMemory()
*
* remark	: Save user data to a file
*******************************************************************/
void freeMemory(){
	//start of the user code
	
	/*int i;
	int height=(col/2);
	for(i=0; i<height; i++)
		free(arr[i]);
	free(arr);*/

	int i,j;
	int height=(col/2);
	if(do_free==1){
		for(i=0; i<height; i++)
			free(arr[i]);
		free(arr);
		
		for(j=0;j<height;j++)
			free(MAZE[j]);
		free(MAZE);
		
		do_free=0;
	}

	if(is_dfs==1)
	{
		for(i=0;i<height;i++){
			free(visited[i]);
		}
		free(visited);
		free(stack);
		is_dfs=0;
		top=-1;
	}

	if(is_bfs==1)
	{
		for(i=0;i<height;i++){
			free(visited[i]);
		}
		free(visited);
		free(queue);
		free(history);
		is_bfs=0;
		rear=-1;
		front=-1;
	}
	//end of usercode
}

/**************************************************************
* function	: bool writeFile(const char* filename)
*
* argument	: const char* filename - filename to be written
* return	: true if success, otherwise false
* remark	: Save user data to a file
****************************************************************/
bool writeFile(const char* filename){
	//start of the user code
	bool flag;
	flag = 0;

	return flag;
	//end of usercode
}

/************************************************************************
* fucntion	: void drawMain(CDC* pDC)
*
* argument	: CDC* pDC - device context object pointer
* remark	: Main drawing function. Called by CMFC_MainView::OnDraw()
*************************************************************************/
void drawMain(CDC *pDC){
	//if direct drawing is defined
#if defined(GRAPHICS_DIRECT)
	drawDirect(pDC);
	//if buffered drawing is defined
#elif defined(GRAPHICS_BUFFERED)
	drawBuffered();
#endif
}

/************************************************************************
* function	: static void drawDirect(CDC *pDC
*
* argument	: CDC* pDC - device context object pointer
* remark	: Direct drawing routines here.
*************************************************************************/
static void drawDirect(CDC *pDC){
	//begin of user code
	//Nothing to write currently.
	//end of user code
}

/***********************************************************************
* function	: static void drawBuffered()
*
* argument	: CDC* pDC -0 device object pointer
* remark	: Buffered drawing routines here.
************************************************************************/
static void drawBuffered(){
	//start of the user code
	int i=0,j=0;
	DrawLine_I(0,0,0,(col/2),4,RGB(181,71,48));
	DrawLine_I(0,0,(row/2),0,4,RGB(181,71,48));
	for(i=0; i<col; i++)
	{
		if(i%2==1)
		{
			for(j=0; j<row; j++)
			{
				if(j%2==1)
				{
					if(MAZE[i/2][j/2].right==1)
					   	DrawLine_I(j/2+1,i/2,j/2+1,i/2+1,4,RGB(181,71,48)); //	DrawLine_l(width+1,height,width+1,height+1,4,RGB(181,71,48));
					if(MAZE[i/2][j/2].down==1)
						DrawLine_I(j/2,i/2+1,j/2+1,i/2+1,4,RGB(181,71,48)); //DrawLine_l(width,height+1,width+1,height+1,4,RGB(181,71,48));
				}
			}
		}
	}

	if(is_dfs==1)
	{	
		DrawSolidBox_I (0 + 0.4, 0 + 0.4, 0 + 0.6, 0 + 0.6, 1,  RGB (223, 206, 190),  RGB (223, 206, 190));

		for(i=0;i<col/2;i++){
			for(j=0;j<row/2;j++){
				if(visited[i][j]!=0)
				{	
					if (MAZE[i][j].up == 0&&visited[i-1][j]!=0)
						DrawSolidBox_I (j + 0.4, i - 0.4, j + 0.6, i + 0.4, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					if (MAZE[i][j].down == 0&&visited[i+1][j]!=0)
						DrawSolidBox_I (j + 0.4, i + 0.6, j + 0.6, i + 1.4, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					if (MAZE[i][j].left == 0&&visited[i][j-1]!=0)
						DrawSolidBox_I (j - 0.4, i + 0.4, j + 0.4, i + 0.6, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					if (MAZE[i][j].right == 0&&visited[i][j+1]!=0)
						DrawSolidBox_I (j + 0.6, i + 0.4, j + 1.4, i + 0.6, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					
					DrawSolidBox_I (j + 0.4, i + 0.4, j + 0.6, i + 0.6, 1,  RGB (223, 206, 190),  RGB (223, 206, 190));				
				}
			}
		}

		DrawLine_I(0+0.5, 0+0.5, stack[0].y+0.5,stack[0].x+0.5,3,RGB(30,30,30));

		for(i=0;i<top;i++)
			DrawLine_I(stack[i].y+0.5, stack[i].x+0.5,stack[i+1].y+0.5,stack[i+1].x+0.5,3,RGB(30,30,30));
		
		DrawSolidBox_I (0 + 0.4, 0 + 0.4, 0 + 0.6, 0 + 0.6, 4, RGB (107, 107, 107), RGB (107, 107, 107));
		DrawSolidBox_I ((row/2)-1+ 0.4, (col/2)-1 + 0.4, (row/2)-1 + 0.6, (col/2)-1 + 0.6, 4, RGB (107, 107, 107), RGB (107, 107, 107));

	}
	
	else
	{
		g_pView->Invalidate();
		do_free=1;
	}

	if(is_bfs==1)
	{	
		DrawSolidBox_I (0 + 0.4, 0 + 0.4, 0 + 0.6, 0 + 0.6, 1,  RGB (223, 206, 190),  RGB (223, 206, 190));

		for(i=0;i<col/2;i++){
			for(j=0;j<row/2;j++){
				if(visited[i][j]!=0)
				{	
					if (MAZE[i][j].up == 0&&visited[i-1][j]!=0)
						DrawSolidBox_I (j + 0.4, i - 0.4, j + 0.6, i + 0.4, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					if (MAZE[i][j].down == 0&&visited[i+1][j]!=0)
						DrawSolidBox_I (j + 0.4, i + 0.6, j + 0.6, i + 1.4, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					if (MAZE[i][j].left == 0&&visited[i][j-1]!=0)
						DrawSolidBox_I (j - 0.4, i + 0.4, j + 0.4, i + 0.6, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					if (MAZE[i][j].right == 0&&visited[i][j+1]!=0)
						DrawSolidBox_I (j + 0.6, i + 0.4, j + 1.4, i + 0.6, 1, RGB (223, 206, 190), RGB (223, 206, 190));
					
					DrawSolidBox_I (j + 0.4, i + 0.4, j + 0.6, i + 0.6, 1,  RGB (223, 206, 190),  RGB (223, 206, 190));				
				}
			}
		}

		DrawLine_I(0+0.5, 0+0.5, history[0].y+0.5,history[0].x+0.5,3,RGB(30,30,30));

		for(i=0;i<n;i++)
			DrawLine_I(history[i].y+0.5, history[i].x+0.5,history[i+1].y+0.5,history[i+1].x+0.5,3,RGB(30,30,30));

		DrawSolidBox_I (0 + 0.4, 0 + 0.4, 0 + 0.6, 0 + 0.6, 4, RGB (107, 107, 107), RGB (107, 107, 107));
		DrawSolidBox_I ((row/2)-1+ 0.4, (col/2)-1 + 0.4, (row/2)-1 + 0.6, (col/2)-1 + 0.6, 4, RGB (107, 107, 107), RGB (107, 107, 107));

	}

	else
	{
		g_pView->Invalidate();
		do_free=1;
	}
	//end of the user code
}
void DFS()
{
	int i,j;
	int height=col/2;
	int width=row/2;
	int total=height*width;
	int popFlag=0;
	store.x=0;
	store.y=0;
	
	visited=(int**)malloc(sizeof(int)*height);
	for(i=0; i<height; i++)
		visited[i]=(int*)malloc(sizeof(int)*width);
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
			visited[i][j]=0;
	}

	stack=(element*)malloc(sizeof(element)*total);
	for(i=0; i<total; i++)
	{
		stack[i].x=0;
		stack[i].y=0;
	}

	i=0; j=0;
	visited[0][0]=1;

	while(1)
	{
		if((i==height-1) && (j==width-1))
		{
			push(i,j);
			break;
		}

		if((MAZE[i][j].down==0) && (visited[i+1][j]==0))
		{
			if(popFlag==1)
			{
				push(i,j);
				popFlag=0;
			}
			i++;
			push(i,j);
			visited[i][j]=1;
		}
		else if((MAZE[i][j].up==0) && (visited[i-1][j]==0))
		{
			if(popFlag==1)
			{
				push(i,j);
				popFlag=0;
			}
			i--;
			push(i,j);
			visited[i][j]=1;
		}
		else if((MAZE[i][j].left==0) && (visited[i][j-1]==0))
		{
			if(popFlag==1)
			{
				push(i,j);
				popFlag=0;
			}
			j--;
			push(i,j);
			visited[i][j]=1;
		}
		else if((MAZE[i][j].right==0) && (visited[i][j+1]==0))
		{
			if(popFlag==1)
			{
				push(i,j);
				popFlag=0;
			}
			j++;
			push(i,j);
			visited[i][j]=1;
		}
		else
		{
			store=pop();
			i=store.x;
			j=store.y;
			popFlag=1;
		}
	}
	is_dfs=1;
	is_bfs=0;
}


void BFS()
{
	int i,j;
	int height=col/2;
	int width=row/2;
	int total=height*width;
	int flag=0;
	int delete_flag=0;

	store.x=0;
	store.y=0;

	visited=(int**)malloc(sizeof(int)*height);
	for(i=0; i<height; i++)
		visited[i]=(int*)malloc(sizeof(int)*width);
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
			visited[i][j]=0;
	}

	queue=(element*)malloc(sizeof(element)*total);
	for(i=0; i<total; i++)
	{
		queue[i].x=0;
		queue[i].y=0;
	}
	
	history=(element*)malloc(sizeof(element)*total);
	for(i=0; i<total; i++)
	{
		history[i].x=0;
		history[i].y=0;
	}

	i=0;
	j=0;
	visited[0][0] = 1;
	addq(i,j);

	while(1)
	{
		store=deleteq();
		i=store.x;
		j=store.y;
		history[n].x = i;
		history[n].y = j;
	
		if(i==height-1 && j==width-1)
		{
			break;
		}

		if(MAZE[i][j].down==0 && visited[i+1][j]==0)
		{	
			addq(i+1,j);
			visited[i+1][j]=1;
			flag=1;
		}
		if(MAZE[i][j].right==0 && visited[i][j+1]==0)
		{
			addq(i,j+1);
			visited[i][j+1]=1;
			flag=1;
		}
		if(MAZE[i][j].up==0 && visited[i-1][j]==0)
		{
			addq(i-1,j);
			visited[i-1][j]=1;
			flag=1;
		}
		if(MAZE[i][j].left==0 && visited[i][j-1]==0)
		{
			addq(i,j-1);
			visited[i][j-1]=1;
			flag=1;
		}
		if (flag==0)
			n--;
		flag=0;
		n++;
	}
	is_bfs=1;
	is_dfs=0;
}