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
    struct Table* hashNext;
}Table;

Table* hashTable[MAX_HASH];

unsigned int key;
char name[MAX];
char format[MAX];
char mnemonic[MAX];

typedef struct Node{
    char data[MAX];
    struct Node* link;
}Node;

Node* head = NULL;
Node* tail = NULL;

signed long start = -1;
signed long end = -1;
signed long value = -1;
signed long address = -1; //보통 메모리 주소는 unsigned지만 예외처리를 위해 음수를 써야했음. 또 FFFFF가 최대니까 범위고 괜찮고..
unsigned int line_num = 0;//가상메모리 해당 라인의 맨 왼쪽 주소
unsigned int dump_p = 0;//가상메모리 각 주소 가리키는 용도
unsigned int temp_p = 0;//현재 dump_p를 저장할 용도 ;reset때 필요;
unsigned char virtual[MAX_memory]="is sample program;;saoo3rklsnkada sd 20161620 suyeo leald;jdasAD";
char command[MAX];///전체 명령어를 관장하는 전역변수 ;; 밑의 arrangeCommand를 거치면 명령어가 정리됨
char store_command[MAX];//히스토리 출력용을 위해서 처음 받은 명령어 저장
char cmp_command[MAX]; //실제 가능한 명령어(비교용)
char store2[MAX]={'\0', };//입력의 앞뒤 자르고 사이 공백은 냅둠, 필요없는 문자도 저장된 상태;
int return_flag2 = 1;
char dumpRange[MAX] = {'\0', };//명령어 뒤에 인자만 받아 저장

Node* Create_Node(char* data);///새로운 노드 생성하고 초기화
void insertBack(Node** head, Node** tail, Node* newnode);///알맞은 명령어일때 히스토리에 넣어줌
int arrangeCommand(char* command);//명령어 각종 예외처리를 하고 정리해줌
void DumpEditFill(int start, int end, int address, int value);//dump, edit, fill 명령어 수행 함수
Table* Create_Table(int key, char* name);///해시테이블에 사용할 새로운 노드 생성하고 초기화
int hashFunc(char* mnemonic);///해시함수

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

    if((store2[i+1] != ' ') && (store2[i+1] != '\t') && (store2[i+1] != '\0'))
	return_flag2 = -1;
    else if(store2[i+1] == '\0')
	return_flag2 = 2;//인자 없이 정상 명령어에서 끝날때 바로 실행

    return i;
}

void cut_space(char* str)
{
    int i, j;
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
