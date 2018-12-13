#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE_STACK 100
#define FALSE 0
#define TRUE 0

typedef struct node{
    char token;
    struct node* link;
}node;

typedef struct stack{
    node* top;
    char token;
    struct node* link;
}stack;

void push(stack* s,char token)
{
    node* new = (node*)malloc(sizeof(node));
    new->token = token;
    new->link = s->top;
    s->top = new;
}

char pop(stack* s)
{
    char re;
    node* new;
    if(is_empty(s))
	exit(1);

    new = s->top;
    re = new->token;
    s->top = new->link;
    free(new);

    return re;
}

void init(stack* s) {
	s->top = NULL;
}

int is_empty(stack* s) {
	return s->top == NULL;
}

int is_full(stack* s) {
	return s->top == MAX_SIZE_STACK - 1;
}

char peek(stack* s) {
	if (is_empty(s))
		exit(1);
	return s->token;
}
int prec(char op) {
	switch (op) {
	case '(': case ')': return 0;
	case '+': case '-': return 1;
	case '*': case '/': return 2;
	}
	return -1;
}

void infix_to_postfix(char exp[]) {
	int i;
	char ch, top_op;
	int len = (int)strlen(exp); //파일에서 저장한 문자열  길이 측정

	stack s;
	init(&s);

	for (i = 0; i<len; i++) {
		ch = (int)exp[i]; //문자열 하나씩 저장해서

		switch (ch) {
		case '+': case '-': case '*': case '/':
			while (!is_empty(&s) && prec(peek(&s)) >= prec(ch)) {
				top_op = pop(&s);
				printf("%c ", top_op); //연산자의 우선순위를 파악해서 스택의 맨위에 넣고 출력
			}
			push(&s, ch); 
			break;

		case '(':
			push(&s, ch); //()의 경우에는 출력은 하지 않고 그 안의 연산자를 스택에 쌓아서 출력
			break;
		case ')':
			top_op = pop(&s);
			while (top_op != '(') {
				printf("%c ", top_op);
				top_op = pop(&s);
			}
			break;

		default:
			printf("%c ", ch); //연산자 아닌 계산하는 수 (a,b,c 등) 출력
			break;
		}
	}

	while (!is_empty(&s)) {
		printf("%c ", pop(&s));
	}
}

void read (FILE *fp, char *expr) //파일받기
{ 
    char c,i=0;
    while(fscanf(fp,"%c ",&c)!=EOF)
    {
	if(c == ' ')
	    continue;
	expr[i]=c;
	i++;
    }
}

int main()
{
    char expr[100];
    FILE *fp = fopen("expr.txt","r");
    read(fp,expr);
    infix_to_postfix(expr);
    printf("\n");
    
    return 0;
}
