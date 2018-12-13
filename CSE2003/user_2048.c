#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <unistd.h>

/* game over flag & board state */
int game_over, b[4][4];
FILE *fp;
//위로 이동하는 함수(user_func_1)//
int move_up (int b[4][4])
{   
    int i,j,temp;
    int count=0;
    for(i=0; i<4; i++)
    {
	for(j=0; j<4; j++)
	{
	    if(b[i][j] != 0)//b[i][j]가 0이 아닐때 아래 숫자와 비교해서 같으면 합쳐주고 아래 배열은 0으로 만들어준다.
	    {
		for(temp=i+1; temp<4; temp++)
		{	
		    if(b[temp][j]==b[i][j])
		    {
			b[i][j] *= 2;
			b[temp][j]=0;
			count++;
			break;
		    }
		    else if(b[temp][j]!=0)
			break;
		}

		for(temp=i-1; temp>=0; temp--)//위의 숫자들을 훑으면서 0이아닌 숫자가 나오면 break한다.
		{
		    if(b[temp][j]!=0)
			break;
		}
		temp++;
		//break한 지점에서 1을 더해준 index가 i와 같지 않으면 위로 올라갈 자리가 있다는 뜻이므로 올려준다.
		if(i!=temp)
		{
		    b[temp][j]=b[i][j];
		    b[i][j]=0;
		    count++;
		}
	    }
	}
    }
    return count; //count는 움직이거나 합쳐진  횟수를 세어준다. 만약 count가 0일 경우 움직이지 않고 합쳐지지 않았다는 뜻이므로 main에서 0을 반환하여 랜덤 숫자를 생성하지 않는다.(과정2의 추가기능 구현)
}

//아래로 이동하는 함수 (user_func_2)//
int move_down(int b[4][4])
{
    int i,j,temp;
    int count=0;
    for(i=3; i>=0; i--)
    {
	for(j=3; j>=0; j--)
	{
	    if(b[i][j] != 0)//b[i][j]와 위의 숫자를 비교해서 같은것이 있으면 합쳐준 후 위 배열은 0으로 만들어준다.
	    {
		for(temp=i-1; temp>=0; temp--)
		{	
		    if(b[temp][j]==b[i][j])
		    {
			b[i][j] *= 2;
			b[temp][j]=0;
			count++;
			break;
		    }
		    else if(b[temp][j]!=0)
			break;
		}

		for(temp=i+1; temp<4; temp++)//아래의 숫자들을 훑으면서 0이 아닌 숫자가 나올때 break해준다.
		{
		    if(b[temp][j]!=0)
			break;
		}
		temp--;
		//break한 지점에서 1을 빼준 index가 i와 같지 않으면 아래로 내려갈 자리가 있다는 뜻이므로 내려준다.    
		if(i!=temp)
		{
		    b[temp][j]=b[i][j];
		    b[i][j]=0;
		    count++;
		}
	    }
	}
    }
    return count;//move_up의 count기능과 같다.
}

//b[][]를 모두 0으로 초기화//
void init(){
	int i, j;
	game_over = 0;
	for(i=0; i<4; i++)
	{
	    for(j=0; j<4; j++)
		    b[i][j] = 0;
	}
}

//시계방향으로 90도 회전//
void rotate(int b[4][4]){
    /* user code */
    int i,j;
    int c[4][4];
    for(i=0; i<4; i++)
    {
	for(j=0; j<4; j++)
	    c[i][j]=b[i][j];
    }//c배열을 하나 선언해서 b를 복사한다.
    init();//b[][]배열을 비운다.
    for(i=0; i<4; i++)
    {
	for(j=0; j<4; j++)
	    b[j][3-i]=c[i][j];//c배열(==b배열)을 90도 돌렸을때를 b에 다시 저장한다.
    }

/*    for(i=0; i<4; i++)
    {
	for(j=0; j<4; j++)
	{
	    printf("%d ",b[i][j]);
	}
	printf("\n");
    }*/
}

//방향키에 따라 이동//
int set_board(int dir, int b[4][4]){
    /* user code init */
    //idea : 배열을 위,아래로 몰아주는 함수(move_up)(move_down)를  구현하면 양옆으로 이동시킬때는 rotate함수를 이용하여 간단히 구현할 수 있다.//
    int flag=1;
    int count;
    if(dir==0)//right키
    {
	rotate(b);
	count=move_down(b);
	rotate(b);
	rotate(b);
	rotate(b);
    }
    else if(dir==1)//up키
    {
	count=move_up(b);
    }

    else if(dir==2)//left키
    {
	rotate(b);
	count=move_up(b);
	rotate(b);
	rotate(b);
	rotate(b);
    }
    else if(dir==3)//down키
    {
	count=move_down(b);
    }
    else//정상 키가 입력되지 않았을때
	flag=0;

    if(flag==1 && count!=0)
	return 1; //정상적인 방향키 입력,움직이거나 합쳐졌음.
    else if(flag ==0 || count==0)
	return 0; //정상적인 방향키 입력이 되지 않음,움직이지도  합쳐지지도 않음.
}

//게임오버 판단//
int is_game_over(){
    /* user code */
    /* if game over return 0, else then return 1 */
    int i,j,count=0;
    int ti,tj;
    int ci,cj;
    int same_flag=0;
    srand(time(NULL));
    for(i=0; i<16; i++)
    {
	if(b[i/4][i%4] !=0)
	    count++;
    }//반복문을 돌리면서 배열이 0이아닐때마다 count++, count가 16이면 꽉 차있는것.
    for(i=0; i<4; i++)
    {
	for(j=0; j<4; j++)
	{
	    ti=i-1;
	    tj=j-1;
	    ci=i+1;
	    cj=j+1;
	    if(ti>0 && b[i][j]==b[ti][j])
		same_flag=1;
	    if(tj>0 && b[i][j]==b[i][tj])
		same_flag=1;
	    if(ci<4 && b[i][j]==b[ci][j])
		same_flag=1;
	    if(cj<4 && b[i][j]==b[i][cj])
		same_flag=1;
	}
    }//반복문을 돌리면서 인접한 위치에 같은 숫자가 있으면 same_flag를 1로 설정
	    
    if(count==16 && same_flag==0)
	return 1;//게임오버이다; 판이 꽉 차있으며 same_flag가 0이다(움직일 수 없다)
    else
	return 0;//게임오버아니다; 판이 꽉 차있지 않거나 same_flag가 1이다(움직일 수 있다)
}

//필드 새로 그리기,점수입력//
void draw_board(int tot, int command){
	int i, j, k, c[8][8], score;
	int tot_count=0;
	/* console clear */
	system("clear");

	score = 0;
	/* user code */
	for(i=0; i<4; i++)//배열에서 기존보다 큰 수가 나올때마다 score에 저장
	{
	    for(j=0; j<4; j++)
	    {
		if(b[i][j]>score)
		    score=b[i][j];
	    }
	}	
	/* calculate score & check sum of all block equasls variable tot */
	for(i=0; i<4; i++)
	{
	    for(j=0; j<4; j++)
	    {
		if(b[i][j] != 0)
		    tot_count++;
	    }
	}
	if (tot_count != tot)
	    printf("tot number error!\n");//반복문을 이용하여 전체 생성된 숫자의 개수를 계산하고 그것이 tot과 같은지 확인한다. 만약 아니면 에러메시지를 출력한다.

	//파일에 입력, 필드 생성(기존 코드)
	printf("    Score : %d\n", score);
	fprintf(fp, "%d %d\n", score, command);

	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			c[i][j] = 32;
			for(k=0; k<50; k++){
				if( b[i][j] == (1<<k) ){
					c[i][j] = 32 + (k%6);
					break;
				}
			}

		}
	}
	for(i=0; i<4; i++){
		for(j=0; j<4; j++)
			fprintf(fp, "%d ", b[i][j]);
		fprintf(fp, "\n");
	}

	for(i=0; i<4; i++){
		puts("");
		puts("");
		for(j=0; j<4; j++)
			printf("\033[%dm%5d\033[0m", c[i][j], b[i][j]);
		puts("");
		puts("");
	}
}

//랜덤으로 2,4 생성//
int make_two_or_four(){
    int num,locate,is_zero=0;
    int arr[3]={2,2,4};//2를 2/3의 확률로, 4를 1/3의 확률로 생성하기 위해 배열 설정
    num=arr[rand()%3];//랜덤으로 수 생성
    while(1)
    {
	locate=rand()%16;//배열 16곳 중에 랜덤으로 위치지정
	if(b[locate/4][locate%4]==0)//그 위치가 0이면
	{
	    is_zero=1;//flag설정하고 break
	    break;
	}
	else//그 위치가 0이 아니라 채워져있으면 다시 랜덤 위치 생성, flag는 0
	    is_zero=0;
    }
    if(is_zero==1)//flag가 1이면 0인곳이 있으므로 생성한 랜덤숫자 전달
	return num;
    else//flag가 0이면 생성할 위치가 없다는 의미이므로 0 전달
	return 0;
    /* user code */
    /* return 2 or 4 that makes in this times */
    /* if can not make two or four, then return 0 */
}

//기존코드//
int getch(void)
{
	char   ch;
	int   error;
	static struct termios Otty, Ntty;

	fflush(stdout);
	tcgetattr(0, &Otty);
	Ntty = Otty;
	Ntty.c_iflag = 0;
	Ntty.c_oflag = 0;
	Ntty.c_lflag &= ~ICANON;
#if 1
	Ntty.c_lflag &= ~ECHO;
#else
	Ntty.c_lflag |= ECHO;
#endif
	Ntty.c_cc[VMIN] = 0;
	Ntty.c_cc[VTIME] = 1;

#if 1
#define FLAG TCSAFLUSH
#else
#define FLAG TCSANOW
#endif

	if (0 == (error = tcsetattr(0, FLAG, &Ntty)))
	{
		error = read(0, &ch, 1);
		error += tcsetattr(0, FLAG, &Otty);
	}

	return (error == 1 ? (int)ch : -1);
}

//기존코드//
int GetCommand() {
	int ch = getch();

	switch (ch)
	{
	case 'd':
	case 'D': return 0;
	case 'w':
	case 'W': return 1;
	case 'a':
	case 'A': return 2;
	case 's':
	case 'S': return 3;
	default: return -1;
	}

	return -1;
}


int main() {
	int command = -1;
	int locate;
	int rand_num;
	fp = fopen("output.txt", "w");
	init();//초기화
	
	srand(time(NULL));

	int tot = 0;
	int i,j;
	/* make init board */
    	rand_num=make_two_or_four();//랜덤으로 생성한 수를 받아온다
	do{
	    locate=rand()%16;
	}while(b[locate/4][locate%4] !=0);//0이 아닌 위치가 나올때까지 위치 랜덤 지정
	b[locate/4][locate%4]=rand_num;//그 위치에 랜덤 숫자 넣어준다
	tot++;

	rand_num=make_two_or_four();
	do{
	    locate=rand()%16;
	}while(b[locate/4][locate%4] !=0);
	b[locate/4][locate%4]=rand_num;//초기화면에 랜덤 숫자가 2개 나와야 하므로 두번 해준다.
	tot++;

	draw_board(tot, -1);//초기화면 그려줌
	do 
	{
	    tot=0;
	    command = GetCommand();//입력키 받아옴
	    if (command != -1) //정상적인 입력일때
	    {	
		if(set_board(command,b))//정상적인 입력이고 움직이거나 합쳐진적이 있을때
		{
		    /* user code */
		    rand_num=make_two_or_four();//난수 생성
		    do{
			locate=rand()%16;
		    }while(b[locate/4][locate%4] !=0);
		    b[locate/4][locate%4]=rand_num;//0이 아닌 랜덤 위치에 난수 지정
		    
		    for(i=0; i<4; i++)
		    {
			for(j=0; j<4; j++)
			{
			    if(b[i][j]!=0)
				tot++;
			}
		    }//draw_board에 tot을 넘겨주면 전체 생성된 숫자의 개수와 같은지 확인한다.
		    draw_board(tot, command);//새로운 난수 포함한 새 배열 그리기
		}
	    }
	    game_over=is_game_over();//게임오버인지 아닌지 판단
	} while (!game_over);//게임 오버일때까지 반복(game_over==1이면 게임오버)

	system("clear");//콘솔 창을 없애고 게임오버메시지  출력
	printf("game over!\n");
	fclose( fp );

	return 0;
}
