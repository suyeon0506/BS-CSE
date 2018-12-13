#include <stdio.h>
#include <direct.h>
#include <stdlib.h>

#define max(x,y) ((x) < (y) ? (y) : (x))
#define swap(x,y) {int t; t=x; x=y; y=t;}

int dynamic_1(char *str, int len);
char* print_1(char *str, char *rev, int len);
int isPalindrome(char* number, int i, int j);
int dynamic_2(char* number, int len, int **L);
////3번문제는 별도함수없이 전부 main에 구현////////

int main(void)
{
	////////////////////////////////////////////1번 문제//////////////////////////////////////////////
	int num, len, result; //num은 문제 수, len은 문자열 길이
	int i, j, k, f;
	char *str, *rev, *result_str;
	char enter; //엔터 저장
	mkdir("20161620_OUTPUT");
	FILE *fp1_in, *fp1_out;
	fp1_in = fopen("INPUT/input-3-1.txt", "r");
	fp1_out = fopen("20161620_OUTPUT/output-3-1.txt","w");
	fscanf(fp1_in, "%d", &num);
	fprintf(fp1_out,"%d\n", num);

	for (i = 0; i < num; i++)
	{
		fscanf(fp1_in, "%d", &len);
		str = (char*)malloc(sizeof(char)*(len + 1));
		rev = (char*)malloc(sizeof(char)*(len + 1));
		fscanf(fp1_in, "%c", &enter); //엔터 저장
		fgets(str, len+1, fp1_in); //문자열 저장
		result = dynamic_1(str, len);

		result_str = (char*)malloc(sizeof(char)*result);
		for (j = 0; j < len; j++)
			rev[j] = str[len - j - 1];
		rev[len] = str[len];

		result_str = print_1(str, rev, len);

		fprintf(fp1_out,"%d\n", result);
		fprintf(fp1_out,"%s\n", result_str);
	}

	fclose(fp1_in);
	fclose(fp1_out);
	free(str);
	free(rev);
	free(result_str);

	//////////////////////////////////////////2번 문제//////////////////////////////////////////////////
	char *number;
	FILE *fp2_in, *fp2_out;
	int **L;
	int *index;
	int temp, flag1;

	fp2_in = fopen("INPUT/input-3-2.txt", "r");
	fp2_out = fopen("20161620_OUTPUT/output-3-2.txt", "w");
	fscanf(fp2_in, "%d", &num);
	fprintf(fp2_out,"%d\n", num);

	for (i = 0; i < num; i++)
	{
		fscanf(fp2_in, "%d", &len);
		number = (char*)malloc(sizeof(char)*len + 1);
		L = (int**)malloc(sizeof(int*)*len);
		for (j = 0; j < len; j++)
			L[j] = (int*)malloc(sizeof(int)*len);
		for (j = 0; j < len; j++)
		{
			for (k = 0; k < len; k++)
				L[j][k] = 0;
		}
		fscanf(fp1_in, "%c", &enter); //엔터 저장
		fgets(number, len + 1, fp2_in); //문자열 저장

		result = dynamic_2(number, len, L);
		fprintf(fp2_out,"%d\n", result);

		index = (int*)malloc(sizeof(int)*result);
		temp = result - 1;
		for (j = len - 1; j >= 0; j--)
		{
			flag1 = 0;
			for (k = 0; k < len; k++)
			{
				if (L[k][j] == 1)
				{	
					flag1 = 0;
					index[temp] = j + 1 - k;
					j = j - index[temp] + 1;
					temp--;
					break;
				}
				else
					flag1 = 1;
			}
			if (flag1 == 1)
			{
				index[temp] = 1;
				temp--;
			}
		}//각 회문의 원소 수 저장한 배열
		
		k = 0;
		for (j = 0; j < result; j++)
		{
			flag1 = k; //여기서는 인덱스 저장용 변수로 사용
			temp = index[j];
			for (; k < temp+flag1; k++)
				fprintf(fp2_out,"%c", number[k]);
			fprintf(fp2_out,"\n");
		}
	}

	fclose(fp2_in);
	fclose(fp2_out);
	free(number);
	free(index);
	free(L);

	//////////////////////////////////3번 문제///////////////////////////////////////////
	FILE *fp3_in, *fp3_out;
	int class_num, max_time, max_term;
	int flag, count, temp_k;
	int *start_time, *finish_time, *time_term;
	int **combine;

	fp3_in = fopen("INPUT/input-3-3.txt", "r");
	fp3_out = fopen("20161620_OUTPUT/output-3-3.txt", "w");
	fscanf(fp3_in, "%d", &num);
	fprintf(fp3_out,"%d\n", num);

	for (i = 0; i < num; i++)
	{
		fscanf(fp3_in, "%d", &class_num);
		start_time = (int*)malloc(sizeof(int)*class_num);
		finish_time = (int*)malloc(sizeof(int)*class_num);
		time_term = (int*)malloc(sizeof(int)*class_num);
		for (j = 0; j < class_num; j++)
		{
			fscanf(fp3_in, "%d", &start_time[j]);
			fscanf(fp3_in, "%d", &finish_time[j]);
			time_term[j] = finish_time[j] - start_time[j];
		}

		max_time = 0;
		for (j = 0; j < class_num; j++)
		{
			if (max_time < finish_time[j])
				max_time = finish_time[j];
		}//끝나는 시간이 제일 늦은것을 고름

		for (j = 0; j < class_num; j++)
		{
			max_term = 0;
			for (k = j; k < class_num; k++)
			{
				if (max_term < time_term[k])
				{
					max_term = time_term[k];
					temp_k = k;
				}
			}
			swap(start_time[j], start_time[temp_k]);
			swap(finish_time[j] , finish_time[temp_k]);
			swap(time_term[j], time_term[temp_k]);
		}//수업시간이 긴 순서로 정렬

		combine = (int**)malloc(sizeof(int)*(max_time+1));
		for (j = 0; j < max_time+1; j++)
			combine[j] = (int*)malloc(sizeof(int)*(max_time+1));
		for (j = 0; j < max_time + 1; j++)
		{
			for (k = 0; k < max_time + 1; k++)
				combine[j][k] = 0;
		}
		
		count = 0;
		for (j = 0; j < class_num; j++)
		{
			flag = 0;
			for (k = 0; k < max_time + 1; k++)
			{
				for (f = start_time[j]; f < finish_time[j]; f++)
				{
					if (combine[k][f] == 0)
						flag = 1;
					else
					{
						flag = 0;
						break;
					}
				}

				if (flag == 1)
				{
					for (f = start_time[j]; f < finish_time[j]; f++)
						combine[k][f] = 1;
				}

				if (flag == 1)
					break;
			}

			if (k > count)
				count = k;
		}

		count++;
		fprintf(fp3_out,"%d\n", count);
	
	}

	fclose(fp3_in);
	fclose(fp3_in);
	free(start_time);
	free(finish_time);
	free(time_term);
	for (i = 0; i < max_time + 1; i++)
		free(combine[i]);
	free(combine);
	///////////////////파일닫고 프리!!!!!!!!!!!!!!!!///////
}

int dynamic_1(char *str, int len)
{
	int i, j, k;
	int **L;
	L = (int**)malloc(sizeof(int*)*len);
	for (i = 0; i < len; i++)
		L[i] = (int*)malloc(sizeof(int)*len);
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < len; j++)
			L[i][j] = 0;
	}
	
	for (i = 0; i < len; i++)
		L[i][i] = 1;

	for (k=1; k<len; k++)
	{
		for(i=0; i<len-k; i++)
		{
			j = i + k;
			if (str[i] != str[j])
				L[i][j] = max(L[i + 1][j], L[i][j - 1]);
			else if ((str[i] == str[j]) && k==1)
				L[i][j] = 2;
			else
				L[i][j] = L[i + 1][j - 1] + 2;
		}
	}

	return L[0][len-1];
}

char* print_1(char *str, char *rev,int len)
{
	int i, j;
	int index;
	char *result;
	int **L;
	L = (int**)malloc(sizeof(int*)*(len+1));
	for (i = 0; i < len+1; i++)
		L[i] = (int*)malloc(sizeof(int)*(len+1));

	for (i = 0; i <= len; i++)
	{
		for (j = 0; j <= len; j++)
		{
			if (i == 0 || j == 0)
				L[i][j] = 0;
			else if (str[i - 1] == rev[j - 1])
				L[i][j] = L[i - 1][j - 1] + 1;
			else
				L[i][j] = max(L[i - 1][j], L[i][j - 1]);
		}
	}

	index = L[len][len];
	result = (char*)malloc(sizeof(char)*(index + 1));
	for (i = 0; i < index + 1; i++)
		result[i] = '\0';
	i = len;
	j = len;

	while (i > 0 && j > 0)
	{
		if (str[i - 1] == rev[j - 1])
		{
			result[index - 1] = str[i - 1];
			i--;
			j--;

			index--;
		}
		else if (L[i - 1][j] >= L[i][j - 1])
			i--;
		else
			j--;
	}

	return result;

}

int isPalindrome(char* number, int i, int j)
{
	while (i < j) {
		if (number[i++] != number[j--])
			return 0;
	}
	return 1;
}

int dynamic_2(char* number, int len, int **L)
{
	int i, j, k, str_len;
	int sum;

	for (str_len= 1; str_len<=len; str_len++)
	{
		for (i = 0; i <= len - str_len; i++) 
		{
			j = i + str_len - 1;
			if (isPalindrome(number, i, j) == 1)
			{
				L[i][j] = 1;
			}
			else
			{           
				L[i][j] = str_len;
				for (k = i; k < j; k++)
				{
					sum = L[i][k] + L[k + 1][j];
					if (sum < L[i][j])
						L[i][j] = sum;
				}
			}
		}
	}

	return L[0][len - 1];
}