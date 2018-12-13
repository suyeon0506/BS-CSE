#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//모든 경로를 이동했는지 검사해줌
int test (char **board, int board_row, int board_col);
//이동한 경로를 화면에 출력해줌
void print(char **board, int board_row, int board_col);
//출구를 계산해줌; 이때 출구란 체스판 위에 있으면서 아직 기사에 의해 방문되지 않은 위치이다
int next (char **board, int board_row, int board_col, int x, int y);
//다음으로 이동이 가능한 위치 배열
int ktmove1[8]={-2,-1,1,2,2,1,-1,-2};
int ktmove2[8]={-1,-2,-2,-1,1,2,2,1};

int main (void){
    int row,col; //행,열 선언
    int board_row, board_col; //보드의 사이즈를 입력
    int sx, sy; //시작점의 x,y저장
    int x,y;  //현재의 x,y저장
    int n;  //다음으로 이동이 가능한 위치를 저장
    char **board =NULL;  //체스판배열
    int exits[8]={0};  //출구의 개수 저장
    int min=8;  //최소의 출구의 개수를 비교하기위해 선언하는 변수
    clock_t start,end;

    printf("배열의 크기는? ");
    scanf("%d %d",&board_row,&board_col);
    //출력범위를 0-63으로 맞춰줌
    if(board_row>8 || board_col>8)
    {	
	printf("8*8이상의 보드는 불가능합니다.\n");
	return 0;
    }

    //사용자가 입력한 크기만큼 동적할당
    board=(char**)malloc(sizeof(char*)*board_row);
    for(row=0; row<board_row; row++)
	board[row]=(char*)malloc(sizeof(char*)*board_col);

    //배열 초기화
    for(row=0;row<board_row;row++)
    {
	for(col=0;col<board_col;col++)
	    board[row][col] = 0;
    }
    
    //배열의 크기보다 작은 시작점을 입력받음
    do{
	printf("시작점은? ");
	scanf("%d %d",&sx,&sy);
	fflush(stdin);
    }while((sx>board_row || sy> board_col));

    //배열은 0부터 시작하므로 입력값에서 1을 빼줌
    x=sx-1;
    y=sy-1;
    //처음 위치에 지나간 흔적 처리
    board[x][y]=1;
    //실행시간을 측정하기 위해 설정
    start=clock();
    //현재위치에서 이동가능한 방향을 검사; 최대 8*8이므로 0-7까지 각각 검사
    //next함수를 이용하여 이동가능한 방향에서 최소의 출구수를 찾음
    do{
	if((x + ktmove1[0] >=0 ) && (y + ktmove2[0] >=0))
	{
	    //이미 방문했던 곳이 아니면 if문 실행
	    if( !board[x+ktmove1[0]][y+ktmove2[0]])
		exits[0]=next(board,board_row,board_col,x+ktmove1[0],y+ktmove2[0]) +1;
	}//이동가능한 방향에서 최소의 출구가 없더라도 현재위치에서는 이동가능하므로 1을 더해준다
	if((x + ktmove1[1] >=0 ) && (y + ktmove2[1] >=0))
	{
	    if( !board[x+ktmove1[1]][y+ktmove2[1]])
		exits[1]=next(board,board_row,board_col,x+ktmove1[1],y+ktmove2[1]) +1;
	}
	if((x + ktmove1[2] <board_row ) && (y + ktmove2[2] >=0))
	{
	    if( !board[x+ktmove1[2]][y+ktmove2[2]])
		exits[2]=next(board,board_row,board_col,x+ktmove1[2],y+ktmove2[2]) +1;
	}
	if((x + ktmove1[3] <board_row ) && (y + ktmove2[3] >=0))
	{
	    if( !board[x+ktmove1[3]][y+ktmove2[3]])
		exits[3]=next(board,board_row,board_col,x+ktmove1[3],y+ktmove2[3]) +1;
	}
	if((x + ktmove1[4] <board_row ) && (y + ktmove2[4] <board_col))
	{
	    if( !board[x+ktmove1[4]][y+ktmove2[4]])
		exits[4]=next(board,board_row,board_col,x+ktmove1[4],y+ktmove2[4]) +1;
	}
	if((x + ktmove1[5] <board_row ) && (y + ktmove2[5] <board_col))
	{
	    if( !board[x+ktmove1[5]][y+ktmove2[5]])
		exits[5]=next(board,board_row,board_col,x+ktmove1[5],y+ktmove2[5]) +1;
	}
	if((x + ktmove1[6] >=0 ) && (y + ktmove2[6] <board_col))
	{
	    if( !board[x+ktmove1[6]][y+ktmove2[6]])
		exits[6]=next(board,board_row,board_col,x+ktmove1[6],y+ktmove2[6]) +1;
	}
	if((x + ktmove1[7] >=0 ) && (y + ktmove2[7] <board_col))
	{
	    if( !board[x+ktmove1[7]][y+ktmove2[7]])
		exits[7]=next(board,board_row,board_col,x+ktmove1[7],y+ktmove2[7]) +1;
	}

	for(row=0; row<8; row++) //최소의 출구를 가지는 다음 위치 선정
	{
	    if((min>exits[row])&&(exits[row] !=0))
	    {
		min=exits[row];
		n=row;  //n은 이동할 위치
	    }
	}
	if(min==8) //min이 8이면 이동가능한 곳이 없는 경우이다
	{
	    printf("출구를 찾을 수 없습니다\n");
	    return 0;
	}
	//현재 위치를 최소의 출구를 가지는 위치로 이동
	x += ktmove1[n];
	y += ktmove2[n];
	board[x][y]=1; //이동한 다음에 board배열에 흔적을 남긴다

	print(board,board_row,board_col); //print함수로 board배열 출력

	printf("x=%d, y=%d\n",x,y); //현재 이동한 위치 출력
	//변수들 초기화
	min=8;
	n=0;
	for(row=0;row<8;row++)
	    exits[row]=0;
    }while(!test(board,board_row,board_col)); //test함수를 통해 모든 체스판을 경유했으면 프로그램을 끝낸다
    
    end=clock();
    printf("실행시간: %f\n",((double)(end-start))/CLOCKS_PER_SEC); //실행시간 측정하여 출력

    //동적할당한 배열 메모리 제거
    for(row=0;row<board_row;row++)
	free(board[row]);
}

int test(char **board,int board_row,int board_col)
{
    int temp=0;
    int row,col;
    for(row=0; row<board_row; row++)
    {
	for(col=0;col<board_col;col++)
	{
	    if(board[row][col])
		temp=1;		    //모든 경로를 경유했을때만 temp변수가 1이됨
	    else
		return 0;
	}
    }

    return temp;
}
//이동경로를 출력하는 함수
void  print(char **board, int board_row, int board_col)
{
    int row,col;
    for(row=0; row<board_row; row++)
    {
	for(col=0; col<board_col; col++)
	    printf("%3d",board[row][col]);

	printf("\n");
    }

    printf("\n");
}
//출구의 개수를 계산하는 함수; main 함수의 알고리즘과 동일
int next (char **board, int board_row, int board_col, int x, int y)
{
    int npos=0;

	if((x + ktmove1[0] >=0 ) && (y + ktmove2[0] >=0))
	{
	    if( !board[x+ktmove1[0]][y+ktmove2[0]])
		npos++;
	}
	if((x + ktmove1[1] >=0 ) && (y + ktmove2[1] >=0))
	{
	    if( !board[x+ktmove1[1]][y+ktmove2[1]])
		npos++;
	}
	if((x + ktmove1[2] <board_row ) && (y + ktmove2[2] >=0))
	{
	    if( !board[x+ktmove1[2]][y+ktmove2[2]])
		npos++;
	}
	if((x + ktmove1[3] <board_row ) && (y + ktmove2[3] >=0))
	{
	    if( !board[x+ktmove1[3]][y+ktmove2[3]])
		npos++;
	}
	if((x + ktmove1[4] <board_row) && (y + ktmove2[4]<board_col))
	{
	    if( !board[x+ktmove1[4]][y+ktmove2[4]])
		npos++;
	}
	if((x + ktmove1[5] <board_row ) && (y + ktmove2[5] <board_col))
	{
	    if( !board[x+ktmove1[5]][y+ktmove2[5]])
		npos++;
	}
	if((x + ktmove1[6] >=0 ) && (y + ktmove2[6] <board_col))
	{
	    if( !board[x+ktmove1[6]][y+ktmove2[6]])
		npos++;
	}
	if((x + ktmove1[7] >=0 ) && (y + ktmove2[7] <board_col))
	{
	    if( !board[x+ktmove1[7]][y+ktmove2[7]])
		npos++;
	}

	return npos;
}

