#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX 257
#define STACK_MAX 1000
#define MAX_memory 1048600
#define MAX_HASH 20
#define HASH_KEY(key) key%MAX_HASH////해시 함수 키값으로의 변환 매크로

typedef struct Table{
    int id;
    char instruct[MAX];
    char format[MAX];
    struct Table* hashNext;
}Table;

Table* hashTable[MAX_HASH];

unsigned int key;
char name[MAX];
char format[MAX];
char mnemonic[MAX];
char fileName[MAX];
char fileName_assem[MAX];
char fileName_assem_store[MAX];
char fileStr[MAX];
char obf1[MAX];
char obf2[MAX];
char obf3[MAX];

typedef struct Node{
    char data[MAX];
    struct Node* link;
}Node;

Node* head = NULL;
Node* tail = NULL;

typedef struct Assem{
    char symbol[MAX];
    char mnemonic[MAX];
    char operand[MAX];
    unsigned int loc;
}Assem;

struct Assem assem;

typedef struct symbolTab{
    unsigned int loc;
    char symbol[MAX];
}symbolTab;

symbolTab symTab[MAX];
int symTab_i;

typedef struct objcode{
    unsigned int opcode;
    unsigned int ni,x,b,p,e;
    unsigned int xbpe;
    unsigned int addr;
    unsigned int toSub;//주소에서 빼줄것 저장;pc거나base거나immediate면 0저장
    unsigned int reg1, reg2;//format2를 위함,예외처리 위해 -1받는경우 있어서 signed
}objcode;

objcode obj;
unsigned int base_relative;
unsigned int pc_relative;

typedef struct storeloc{
    unsigned int loc;
    int RES_flag;
}storeloc;
storeloc storeLoc[MAX];

unsigned int storeLoc_i;//loc을 저장하고 특히 변수가 저장되었던 loc엔 flag로 표시해서 obj파일 만들때 length에 들어가지 않게 처리
char TR[MAX];

typedef struct Mod{
    unsigned int loc;
    unsigned int val;
}Mod;

Mod mod[MAX];
int mod_i;

typedef struct ESTAB{
    char csname[MAX];
    char symname[MAX];
    unsigned int addr;
    unsigned int len;
}ESTAB;

ESTAB estab[MAX];
int estab_i;//loader 과정의 estable 선언

typedef struct EXTREF{
    char exname[MAX];
    unsigned int refnum;
}EXTREF;

EXTREF extref[MAX];
int extref_i;//외부참조 저장용

signed long start = -1;
signed long end = -1;
signed long value = -1;
signed long address = -1; //보통 메모리 주소는 unsigned지만 예외처리를 위해 음수를 써야했음. 또 FFFFF가 최대니까 범위고 괜찮고..
unsigned int line_num = 0;//가상메모리 해당 라인의 맨 왼쪽 주소
unsigned int dump_p = 0;//가상메모리 각 주소 가리키는 용도
unsigned int temp_p = 0;//현재 dump_p를 저장할 용도 ;reset때 필요;
unsigned char virtual[MAX_memory];
char command[MAX];///전체 명령어를 관장하는 전역변수 ;; 밑의 arrangeCommand를 거치면 명령어가 정리됨
char store_command[MAX];//히스토리 출력용을 위해서 처음 받은 명령어 저장
char cmp_command[MAX]; //실제 가능한 명령어(비교용)
char store2[MAX]={'\0', };//입력의 앞뒤 자르고 사이 공백은 냅둠, 필요없는 문자도 저장된 상태;
int return_flag2 = 1;
char dumpRange[MAX] = {'\0', };//명령어 뒤에 인자만 받아 저장
int oprComma_flag;//operand가 두개 이상이어서 콤마가 있는경우
unsigned int progaddr = 0;
unsigned int csaddr;
unsigned int execaddr;//프로젝트 3에서 필요한 주소들 선언

Node* Create_Node(char* data);///새로운 노드 생성하고 초기화
int arrangeCommand(char* command);//명령어 각종 예외처리를 하고 정리해줌
void DumpEditFill(int start, int end, int address, int value);//dump, edit, fill 명령어 수행 함수
Table* Create_Table(int key, char* name, char* format);///해시테이블에 사용할 새로운 노드 생성하고 초기화
int hashFunc(char* mnemonic);///해시함수

/////assemble 함수//////////////////////////////////
int makeLST(char* fileName_assem);//lst파일 만들어줌
////////////////////////////////////////////////////

int pass1_load(char* obf);//obf이름의 obj파일을 전달하여 로드 pass1과정 수행
int pass2_load(char* obf);//obf이름의 obj파일을 전달하여 로드 pass2과정 수행

int check_store2(char* cmp_command, char* store2)
{
    int i;
    
    return_flag2 = 1;

    for(i=0;(unsigned)i<strlen(store2); i++)
    {
	if(store2[i]==cmp_command[strlen(cmp_command)-1])
		break;
    }
    
    if(!strcmp(cmp_command,"fill"))
	i++;//fill일때는 끝에 엘이 두개라서 i에 1 추가해준다.
    if(!strcmp(cmp_command,"assemble"))
	i = i+4;//assemble일 경우도 e가 중간에 있기때문에 끝의 e로 i값을 증가시켜준다.
    if(!strcmp(cmp_command,"progaddr"))
	i = i+6;//progaddr일 경우에 r이 중간에 있기때문에 끝의 r로 i값을 증가시켜준다.

    if((store2[i+1] != ' ') && (store2[i+1] != '\t') && (store2[i+1] != '\0'))
	return_flag2 = -1;
    else if(store2[i+1] == '\0')
	return_flag2 = 2;//인자 없이 정상 명령어에서 끝날때 바로 실행

    return i;
}

void cut_space(char* str)
{
    int i=0, j=0;
    for(i=0;(unsigned)i<strlen(str); i++)
    {
	if(str[i] !=' ' && str[i] != '\t')
	{
	    j=i;
	    break;
	}
    }
    strcpy(str,str+j);//앞에 공백 잘라냄
    
    for(i=strlen(str)-1;i>=0 ; i--)
    {
	if(str[i] == ' ' || str[i] == '\t')
	    str[i]='\0';
	else
	    break;
    } //뒤에 공백 잘라냄
}

void insertBack(Node** head, Node** tail, Node* newnode)
{
    if((*head) == NULL)
    {
	(*head) = newnode;
	(*tail) = newnode;
    }
    else
    {
	(*tail)->link = newnode;
	(*tail) = newnode;
    }
}//히스토리에 이용하기 위한 연결리스트 삽입 함수;;알맞은 명령어일때 넣어줌

int findSpace(char* fileStr, int pass_num)
{
    int i, j, temp_in;
    int flag, passComma, oprBYTE_flag;
    char storeSTR[MAX] = {'\0',};
    char store[MAX] = {'\0',};
    char temp[MAX] = {'\0',};
    int number = 0;

    cut_space(fileStr);
    strcpy(storeSTR,fileStr);
    oprComma_flag = 0;
    oprBYTE_flag = 0;
    j=0;
    for(i=0; (unsigned)i<strlen(fileStr); i++)
    {
	if(fileStr[i]==' ' || fileStr[i]=='\t')
	    continue;
	else
	{
	    temp[j] = fileStr[i];
	    j++;
	}
    }

    if(strstr(temp,","))
	oprComma_flag = 1;
    if(strstr(temp,"\'"))
	oprBYTE_flag = 1;

    if(pass_num == 1)
    {
	while(strlen(fileStr) != 0)
	{
	    for(i=0; i<MAX; i++)
		store[i] = '\0';

	    cut_space(fileStr);
	
	    passComma = 0;
	    number++;
	    j=0;
	    for(i=0; (unsigned int)i<strlen(fileStr); i++)
	    {
		if(fileStr[0] == '.')
		    return 0;//주석일때 예외처리

		if((fileStr[i] == ' ' || fileStr[i] == '\t'))
		{
		    flag = 0;
		    for(temp_in = i+1; (unsigned int)temp_in<strlen(fileStr); temp_in++)
		    {
			if(fileStr[temp_in] != ' ' && fileStr[temp_in] != '\t' && fileStr[temp_in] != ',')
			    flag = 1;
		    }
		    
		    if(((number == 2 && (flag == 0 || passComma == 1))|| number == 3) && (oprComma_flag == 1 || oprBYTE_flag == 1))
		    {
			store[j] = fileStr[i];
			j++;
		    }
		    else
			break;
		}
		else
		{
		    if(fileStr[i] == ',')
			passComma = 1;
		    store[j] = fileStr[i];
		    j++;
		}
	    }
	
	    if(number == 1)
		strcpy(assem.symbol, store);
	    else if(number == 2)
		strcpy(assem.mnemonic, store);
	    else if(number == 3)
		strcpy(assem.operand, store);
	    else if(number > 3)
	    {
		printf("%d정상 asm 파일이 아닙니다. 디버깅 할 수 없습니다.\n",number);
		return -1;
	    }

	    strcpy(fileStr,fileStr+i);
	}

	if(number == 2)//symbol이 없을 때
	{
	    strcpy(assem.operand,assem.mnemonic);
	    strcpy(assem.mnemonic,assem.symbol);
	    strcpy(assem.symbol,"\0");
	}
	else if(number == 1)//한 단어이면 mnemonic임
	{
	    strcpy(assem.mnemonic,assem.symbol);
	    strcpy(assem.symbol,"\0");
	}

	strcpy(fileStr,storeSTR);
    }
    else if(pass_num == 2)
    {
	while(strlen(fileStr) != 0)
	{
	    for(i=0; i<MAX; i++)
		store[i] = '\0';

	    cut_space(fileStr);
	
	    number++;
	    passComma = 0;
	    j=0;
	    for(i=0; (unsigned)i<strlen(fileStr); i++)
	    {
		if(fileStr[0] == '.')
		    return 0;//주석일때 예외처리

		if(fileStr[i] == ',')
		    passComma = 1;

		if((fileStr[i] == ' ' || fileStr[i] == '\t'))
		{
		    flag = 0;
		    for(temp_in = i+1; (unsigned int)temp_in<strlen(fileStr); temp_in++)
		    {
			if(fileStr[temp_in] != ' ' && fileStr[temp_in] != '\t' && fileStr[temp_in] != ',')
			    flag = 1;
		    }

		    if(((number == 3 && (flag == 0 || passComma == 1))|| number == 4) && (oprComma_flag == 1 || oprBYTE_flag == 1))
		    {
			store[j] = fileStr[i];
			j++;
		    }
		    else
			break;
		}
		else
		{
		    if(fileStr[i] == ',')
			passComma = 1;
		    store[j] = fileStr[i];
		    j++;
		}
	    }
	
	    if(number == 1)
	    {
		if(!strcmp(store,"BASE") || !strcmp(store,"END"))
		{
		    strcpy(assem.mnemonic,store);
		    number += 2;
		}//base와 end의 경우에는 lco이 없어서 바로 나오는것 처리 필요
		else
		    assem.loc = strtoul(store,NULL,16);
	    }
	    else if(number == 2)
		strcpy(assem.symbol,store);
	    else if(number == 3)
		strcpy(assem.mnemonic, store);
	    else if(number == 4)
		strcpy(assem.operand, store);
	    else if(number > 4)
	    {
		printf("정상 asm 파일이 아닙니다. 디버깅 할 수 없습니다.\n");
		return -1;
	    }

	    strcpy(fileStr,fileStr+i);
	}

	if(number == 3)//symbol이 없을 때
	{
	    strcpy(assem.operand,assem.mnemonic);
	    strcpy(assem.mnemonic,assem.symbol);
	    strcpy(assem.symbol,"\0");
	}
	else if(number == 2)//한 단어이면 mnemonic임 + END 와 BASE 일 때는 loc이 없어서 두단어임
	{
	    strcpy(assem.mnemonic,assem.symbol);
	    strcpy(assem.symbol,"\0");
	}

	strcpy(fileStr,storeSTR);
    }
    return 1;
}

int find_reg_num(char* operand)
{
    if(!strcmp(operand,"A"))
	return 0;
    else if(!strcmp(operand,"X"))
	return 1;
    else if(!strcmp(operand,"L"))
	return 2;
    else if(!strcmp(operand,"PC"))
	return 8;
    else if(!strcmp(operand,"SW"))
	return 9;
    else if(!strcmp(operand,"B"))
	return 3;
    else if(!strcmp(operand,"S"))
	return 4;
    else if(!strcmp(operand,"T"))
	return 5;
    else if(!strcmp(operand,"F"))
	return 6;
    else
	return 100;
}
