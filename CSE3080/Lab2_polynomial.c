#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_TERMS 100
void padd(int startA,int finishA,int startB,int finishB,int *starD,int *finishD);
void attach(float coefficient, int exponent);
int COMPARE(int x,int y);

typedef struct {
    float coef;
    int expon;
} polynomial;
polynomial *terms;
int avail=0;

int main (void)
{
    int startD,finishD;
   // polynomial A[100],B[100];
    int startA,finishA,startB,finishB;

    terms = (polynomial*)malloc(sizeof(polynomial)*100);
    
    FILE *f =fopen("A.txt","r");
    while(1)
    {
	if(fscanf(f,"%f %d",&terms[avail].coef,&terms[avail].expon)==EOF)
	    break;
	else
	    avail++;
    }
    startA = 0;
    finishA= avail-1;
    startB = avail;

    FILE *fp =fopen("B.txt","r");
    while(1)
    {
	if(fscanf(fp,"%f %d",&terms[avail].coef,&terms[avail].expon)==EOF)
	    break;
	else
	    avail++;
    }
    
    finishB=avail-1;
    int k;
    padd(startA,finishA,startB,finishB,&startD,&finishD);
    
    for(k= startD ;k< finishD ;k++)
     printf("(%.0f, %d), ",terms[k].coef,terms[k].expon);

    printf("(%.0f, %d)\n",terms[k].coef, terms[k].expon);

    return 0;
}

int COMPARE(int x,int y)
{
    if(x<y) 
	return -1;
    else if(x==y) 
	return 0;
    else 
	return 1;
}

void padd(int startA, int finishA,int startB,int finishB,int *startD,int *finishD)
{
    float coefficient;
    *startD = avail;
    while(startA <= finishA && startB <= finishB){
	switch (COMPARE(terms[startA].expon,terms[startB].expon))
	{
	    case -1:
		attach(terms[startB].coef,terms[startB].expon);
		startB++;
		break;
	    case 0:
		coefficient = terms[startA].coef+terms[startB].coef;
		if(coefficient)
		    attach(coefficient,terms[startA].expon);
		startA++;
		startB++;
		break;
	    case 1:
		attach(terms[startA].coef,terms[startA].expon);
		startA++;
		break;
	}
    }
    for(;startA <= finishA; startA++)
	attach(terms[startA].coef,terms[startA].expon);
    
    for(;startB <= finishB; startB++)
	attach(terms[startB].coef,terms[startB].expon);

    *finishD = avail-1;
}

void attach(float coefficient,int exponent)
{
    if(avail>=MAX_TERMS)
    {
	printf("%d\n",avail); 
	fprintf(stderr,"다항식에 항이 너무 많다.");
	exit(1);
    }
    terms[avail].coef=coefficient;
    terms[avail++].expon = exponent;
}

			

