#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include "..\ExternDoc.h"
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
	int i;
	int height=(col/2);
	for(i=0; i<height; i++)
		free(arr[i]);
	free(arr);
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
	//end of the user code
}
void DFS()
{

}
void BFS()
{

}