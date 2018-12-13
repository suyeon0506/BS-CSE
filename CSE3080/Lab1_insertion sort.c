#include <stdio.h>
#include <stdlib.h>

int main (void)
{
    int num;
    FILE *fp;
    int *arr;
    int i,j,temp;

    fp = fopen("lab1.data","r");
    fscanf(fp,"%d",&num);
    arr=(int*)malloc(sizeof(int)*num);
    
    for(i=0; i<num; i++)
	fscanf(fp,"%d",&arr[i]);
    
    for(i=1; i<num; i++)
    {
	j=i-1;
	temp=arr[i];

	while(j>=0 && arr[j] > temp)
	{
	    arr[j+1] = arr[j];
	    j=j-1;
	}

	arr[j+1] = temp;
    }

    for(i=0;i <num; i++)
	printf("%d ",arr[i]);

    printf("\n");

    return 0;
}

