#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();
	while(!exit){
		//createRankList();
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_REC: recommendedPlay(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	
	recRoot = (RecNode*)malloc(sizeof(RecNode));
	recRoot->lv = -1;
	recRoot->score = 0;
	for(i=0; i<HEIGHT; i++)
		for(j=0; j<WIDTH; j++)
			recRoot->f[i][j] = field[i][j];
	MakeRecTree(recRoot);
	recommend(recRoot,0);

	DrawOutline();
	//DrawField();
	//DrawShadow(blockY, blockX, nextBlock[0], blockRotate);
	//DrawBlock(blockY,blockX,nextBlock[0],blockRotate,'R');
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	
	//과제추가//
	move(8, WIDTH+10);
	DrawBox(9, WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				attron(COLOR_PAIR(field[j][i]));
				printw(" ");
				attroff(COLOR_PAIR(field[j][i]));
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				attron(COLOR_PAIR(nextBlock[1]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[1]+1));
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	//과제 추가부분//
	for (i=0; i<4; i++)
	{
		move(10+i, WIDTH+13);
		for(j=0; j<4; j++)
		{
			if(block[nextBlock[2]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				attron(COLOR_PAIR(nextBlock[2]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[2]+1));
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				attron(COLOR_PAIR(nextBlock[0]+1));
				printw("%c",tile);
				attroff(COLOR_PAIR(nextBlock[0]+1));
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('o');
	//addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
	//	addch(ACS_HLINE);
	//addch(ACS_URCORNER);
	addch('-');
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		//addch(ACS_VLINE);
		addch('|');
		move(y+j+1,x+width+1);
		//addch(ACS_VLINE);
		addch('|');
	}
	move(y+j+1,x);
	//addch(ACS_LLCORNER);
	addch('o');
	for(i=0;i<width;i++)
		addch('-');
	//	addch(ACS_HLINE);
	//addch(ACS_LRCORNER);
	addch('o');
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			if(block[currentBlock][blockRotate][i][j])
			{
				if((blockY+i>=HEIGHT) || (blockX+j)<0 || (blockX+j>=WIDTH))
					return 0;
				if(f[blockY+i][blockX+j] != 0)
					return 0;
			}
		}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int tempBlock, tempUp, x, y;
	tempBlock = currentBlock;
	tempUp = blockRotate;
	y = blockY; x = blockX;

	switch(command)
	{
		case KEY_UP:
			tempUp = (tempUp+3)%4;
			break;
		case KEY_DOWN:
			y--;
			break;
		case KEY_LEFT:
			x++;
			break;
		case KEY_RIGHT:
			x--;
			break;
	}

	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			if(block[tempBlock][tempUp][i][j] == 1)
			{
				if(y+i>=0)
				{
					move(i+y+1, j+x+1);
					printw(".");
				}
			}
		}
	}
	//DrawBlock(blockY,blockX,currentBlock,blockRotate,' ');
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT, WIDTH+10);
}

void BlockDown(int sig){
	// user code
	timed_out = 0;
	int check;
	int i,j;
	check = CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX);

	if(check ==1)
	{
		blockY = blockY + 1;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else if(check !=1 && blockY == -1)
		gameOver = 1;
	else if(check !=1 && blockY != -1)
	{
		
		score = score + AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		//DrawNextBlock(nextBlock);
		score = score + DeleteLine(field);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand()%7;
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;

		////추가/////
		recRoot->score =0;
		recRoot->curBlockID = nextBlock[0];
		recRoot->recBlockRotate = blockRotate;
		recRoot->recBlockY = blockY;
		recRoot->recBlockX = blockX;
		for(i=0; i<HEIGHT; i++)
			for(j=0; j<WIDTH; j++)
				recRoot->f[i][j] = field[i][j];
		recommend(recRoot, 0);
		////////////////

		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawNextBlock(nextBlock);
		PrintScore(score);
	}
}
void BlockDownREC(int sig)
{
	int i,j;
	timed_out = 0;

	if(recRoot->recBlockY == -1)
	{
		gameOver = 1;
	}
	else
	{
		score += AddBlockToField(field, nextBlock[0], recRoot->recBlockRotate, recRoot->recBlockY, recRoot->recBlockX);
		score += DeleteLine(field);

		for(i=0; i<BLOCK_NUM -1 ; i++)
		{
			nextBlock[i] = nextBlock[i+1];
		}
		nextBlock[BLOCK_NUM - 1] = rand()%7;

		blockRotate = 0;

		blockY = -1;
		blockX = WIDTH/2 -2;
		recRoot->score = 0;
		recRoot->recBlockRotate = blockRotate;
		recRoot->recBlockY = blockY;
		recRoot->recBlockX = blockX;
		for(i=0; i<HEIGHT; i++)
			for(j=0; j<WIDTH; j++)
				recRoot->f[i][j] = field[i][j];
		recommend(recRoot, 0);

		//DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawField();
		DrawShadow(blockY, blockX, nextBlock[0], blockRotate);
		DrawBlock(blockY, blockX, nextBlock[0], blockRotate, 'R');
		DrawNextBlock(nextBlock);
		PrintScore(score);
	}
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int touch = 0;
	for(i=0 ; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			if(block[currentBlock][blockRotate][i][j] == 1)
			{
				if(f[blockY+i+1][blockX+j] || blockY+i+1>=HEIGHT)
					touch=touch+1;
				f[blockY+i][blockX+j] = 1;
			}
		}
	}
	return (touch*10);
	//score = score + (touch*10);
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i,j, k, temp;
	int delete_score =0;
	for(i=HEIGHT-1; i>=0; i--)
	{
		temp=0;
		for(j=0; j<WIDTH; j++)
		{
			if(field[i][j] !=1)
			{
				temp=1;
				break;
			}
		}

		if(temp !=1)
		{
			for(k=i; k>0; k--)
			{
				for(j=0; j<WIDTH; j++)
					f[k][j] = f[k-1][j];
			}

			DrawField();
			i++;
			delete_score = delete_score+1;
		}
	}

	return 100*delete_score*delete_score;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int check;
	int i;

	for(i=y; i<HEIGHT; i++)
	{
		check=CheckToMove(field, blockID, blockRotate, i, x);
		if(check==0)
			break;
	}
	DrawBlock(i-1, x, blockID, blockRotate, '/');
}

//과제 추가//
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	DrawField();
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// user code
	FILE *fp;
	int i, rank_score;
	char name[NAMELEN];
	fp = fopen("rank.txt","r");
	fscanf(fp, "%d", &rank_num);

	for(i=0; i<rank_num; i++)
	{
		fscanf(fp, "%s%d",name,&rank_score);
		NODE* new=NULL;
		new=(NODE*)malloc(sizeof(NODE));
		strcpy(new->name,name);
		new->score = rank_score;
		new->link = NULL;

		if(head==NULL)
			head = new;
		else
		{
			ptr ptr1 = head;
			while(ptr1->link !=NULL)
				ptr1=ptr1->link;
			ptr1->link = new;
		}
	}
		fclose(fp);
}

void rank(){
	// user code
	char num;
	int i,j, X = 1, Y = rank_num, k;
	int delete_num;
	char name[NAMELEN];
	ptr ptr1;
	ptr ptr2;
	ptr new = head;
	int temp2, temp3;
	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	num = getch();
	switch(num)
	{
		case '1':
			echo();
			printw("X: ");
			scanw("%d",&X);
			printw("Y: ");
			scanw("%d",&Y);
			noecho();
			k = Y-X+1;
			printw("          name          |   score   \n");
			printw("------------------------------------\n");
			if(k>0 && X<=rank_num && Y<=rank_num)
			{
				//k = Y-X+1;
				for(i=0; i<X-1; i++)
					new = new->link;
				for(j=0; j<k; new=new->link)
				{
					printw(" %-22s | %d\n",new->name, new->score);
					j++;
				}
			}
			else
				printw("\nsearch failure: no rank in the list\n");
			
			getch();
			break;

		case '2':
			temp2=0;
			ptr1 = head;
			echo();
			printw("input the name: ");
			getstr(name);
			noecho();
			printw("          name          |   score   \n");
			printw("------------------------------------\n");
			for(; ptr1 != NULL; ptr1 = ptr1->link)
			{
				if(strcmp(ptr1->name, name)==0)
				{
					printw(" %-22s | %d\n", ptr1->name, ptr1->score);
					temp2=1;
				}
			}

			if(temp2==0)
				printw("\nsearch failure: no name in the list\n");

			getch();
			break;

		case '3':
			temp3=0;
			ptr1=head;
			ptr2=head;
			echo();
			printw("input the rank: ");
			scanw("%d",&delete_num);
			noecho();
			if(delete_num > rank_num || delete_num < 1)
				printw("\nsearch failure: no rank in the list\n");
			else
			{
				for(i=0; i<delete_num-1; i++)
				{
					ptr2 = ptr1;
					ptr1 = ptr1->link;
					temp3=1;
				}

				if(temp3==0)
					head = ptr1->link;
				else
					ptr2->link = ptr1->link;

				free(ptr1);
				printw("\nresult: the rank deleted\n");
				rank_num = rank_num-1;
				writeRankFile();
			}

			getch();
			break;
	}
}

void writeRankFile(){
	// user code
	int i;
	FILE *fp;
	ptr new = head;
	fp =fopen("rank.txt","w");
	fprintf(fp,"%d\n",rank_num);
	for(;new != NULL; new = new->link)
	{
		fprintf(fp,"%s ",new->name);
		fprintf(fp,"%d\n",new->score);
	}
	fclose(fp);
}

void newRank(int score){
	// user code
	ptr ptr1=head;
	ptr ptr2=head;
	ptr new;
	int i;
	char name[NAMELEN];
	new = (ptr)malloc(sizeof(NODE));
	clear();
	echo();
	printw("your name: ");
	getstr(name);
	noecho();
	new->score = score;
	new->link = NULL;
	strcpy(new->name, name);
	if(head !=NULL)
	{
		for(i=0; i<rank_num; i++)
		{
			if(ptr1->score > new->score)
			{
				ptr2 = ptr1;
				ptr1 = ptr1->link;
			}
			else
				break;
		}
		
		if(i==0)
			head = new;
		else
		{
			new->link = ptr1;
			ptr2->link = new;
		}
	}
	else
		head = new;

	rank_num= rank_num+1;
	writeRankFile();
	noecho();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	int i=0;
	pnode ptr;
	ptr = maxNODE;
	for (i=0; i<VISIBLE_BLOCKS-1; i++)
		ptr = ptr->parent;
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int recommend(RecNode *root, int lv){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code
	int r,x,y,i,j,k,temp_score,temp_max,n=0;
	RecNode **child = root->c;
	if(lv > VISIBLE_BLOCKS - 1)
		return 0;
	for( r =0; r< NUM_OF_ROTATE; r++)
	{
		if(nextBlock[lv] == 4 && r>0)
			break; // 모양이 같은 경우
		else if ((nextBlock[lv] == 0 || nextBlock[lv] == 5 || nextBlock[lv]==6) && r>1)
			break;

		//모든 범위의 x에 대해서 체크한다
		for(x=-3; x<WIDTH; x++)
		{
			y = blockY;
			temp_score=0;
			if(CheckToMove(root->f, nextBlock[lv], r, y, x))
			{
				// 갈수있는 최대한 아래의 y값을 찾는다
				while (CheckToMove(root->f, nextBlock[lv], r, y+1, x))
					y++;
				//부모 노드의 필드 정보를 자식 노드에 입력
				for(i=0; i<HEIGHT; i++)
					for(j=0; j<WIDTH; j++)
						child[n]->f[i][j] = root->f[i][j];

				child[n]->curBlockID = nextBlock[lv];
				child[n]->recBlockX = x;
				child[n]->recBlockY = y;
				child[n]->recBlockRotate =r;
				child[n]->score = root->score + AddBlockToField(child[n]->f, nextBlock[lv], r, y, x);
				child[n]->score += DeleteLine(child[n]->f);

				if(lv == VISIBLE_BLOCKS-1)
					max = child[n] ->score;
				else
				{
					//다음 블럭에 대한 최대 기대값을 구한다
					temp_max = recommend(child[n],lv+1);
					if(max < temp_max)
					{
						max = temp_max;
						if(lv==0)
						{
							recRoot->curBlockID = nextBlock[lv];
							recRoot->recBlockX = x;
							recRoot->recBlockY = y;
							recRoot->recBlockRotate = r;
						}
					}
					else if (max == temp_max)
					{
						if(recRoot->recBlockY < y)
						{
							max = temp_max;
							if(lv ==0)
							{
								recRoot->curBlockID = nextBlock[lv];
								recRoot->recBlockX = x;
								recRoot->recBlockY = y;
								recRoot->recBlockRotate =r;
							}
						}
					}
				}

				n++;
			}
		}
	}
	return max;
}

void recommendedPlay(){
	// user code
	int command;
	clear();
	act.sa_handler = BlockDownREC;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();

	do{
		if(timed_out == 0)
		{
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if(command == QUIT)
		{
			alarm(0);
			DrawBox(HEIGHT/2-1, WIDTH/2-5, 1, 10);
			move(HEIGHT/2, WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1, WIDTH/2-5, 1, 10);
	move(HEIGHT/2, WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();

}

void MakeRecTree(RecNode *root)
{
	int i;
	RecNode **child = root->c;
	for(i=0; i<CHILDREN_MAX; ++i)
	{
		child[i] = (RecNode*)malloc(sizeof(RecNode));
		child[i]->lv = root->lv +1;
		child[i]->parent = root;
		if(child[i]->lv < VISIBLE_BLOCKS-1)
			MakeRecTree(child[i]);
	}
}

