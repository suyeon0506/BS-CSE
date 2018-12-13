#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>

using namespace std;
using namespace chrono;

void algo1(char *file_input, char *file_output);
void algo2(char *file_input, char *file_output);
int MaxSubSum(int *arr, int left, int right);
void algo3(char *file_input, char *file_output);

int left2, right2;//두번째 알고리즘에서 구간을 넘겨주기 위해 사용하는 전역변수

int main(void)
{
	int i, j;
	char read_file[51], file_input[21], file_output[21], version[3];
	FILE *fp;
	fp = fopen("HW1_MSS_config.txt", "r");
	while (!feof(fp))
	{
		fgets(read_file, 50, fp);
		j = 0;
		for (i = 0; read_file[i] != ' '; i++)
		{
			file_input[j] = read_file[i];
			j++;
		}
		file_input[j] = '\0';
		i++;
		j = 0;
		for (; read_file[i] != ' '; i++)
		{
			file_output[j] = read_file[i];
			j++;
		}
		file_output[j] = '\0';
		i++;
		j = 0;
		for (; read_file[i] != '\n' && read_file[i] != '\0'; i++)
		{
			version[j] = read_file[i];
			j++;
		}
		version[j] = '\0';

		if (version[1] == '1')
			algo1(file_input, file_output);
		else if (version[1] == '2')
			algo2(file_input, file_output);
		else
			algo3(file_input, file_output);
	}

	fclose(fp);
	return 0;
}

void algo1(char *file_input, char *file_output)
{
	int num, i, j, *arr;
	int ThisSum, MaxSum = 0;
	int left, right;
	FILE *fp1, *fp2;

	fp1 = fopen(file_input, "rb");
	fread(&num, 4, 1, fp1);
	arr = (int*)malloc(sizeof(int)*num);
	for (i = 0; i < num; i++)
	{
		fread(&arr[i], 4, 1, fp1);
	}
	fclose(fp1);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	for (i = 0; i < num; i++)
	{
		ThisSum = 0;
		for (j = i; j < num; j++)
		{
			ThisSum += arr[j];
			if (ThisSum > MaxSum)
			{
				MaxSum = ThisSum;
				left = i;
				right = j;
			}
		}
	}

	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	chrono::duration<double, milli> time = std::chrono::duration_cast<chrono::duration<double, milli>>(end - start);

	if (MaxSum == 0)
	{
		left = -1;
		right = -1;
	}

	fp2 = fopen(file_output, "w");
	fprintf(fp2, "%d\n", num);
	fprintf(fp2, "1\n");
	fprintf(fp2, "%d\n", MaxSum);
	fprintf(fp2, "%d %d\n", left, right);
	fprintf(fp2, "%.4lf\n", time);
	fclose(fp2);
	free(arr);
}

void algo2(char *file_input, char *file_output)
{
	int i, j;
	int num, *arr, MaxSum;
	FILE *fp1, *fp2;

	fp1 = fopen(file_input, "rb");
	fread(&num, 4, 1, fp1);
	arr = (int*)malloc(sizeof(int)*num);
	for (i = 0; i < num; i++)
	{
		fread(&arr[i], 4, 1, fp1);
	}
	fclose(fp1);

	left2 = 0;
	right2 = num - 1;

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	MaxSum = MaxSubSum(arr, 0, num-1);
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	chrono::duration<double, milli> time = std::chrono::duration_cast<chrono::duration<double,milli>>(end - start);

	if (MaxSum == 0)
	{
		left2 = -1;
		right2 = -1;
	}

	fp2 = fopen(file_output, "w");
	fprintf(fp2, "%d\n", num);
	fprintf(fp2, "2\n");
	fprintf(fp2, "%d\n", MaxSum);
	fprintf(fp2, "%d %d\n", left2, right2);
	fprintf(fp2, "%.4lf\n", time);
	fclose(fp2);
	free(arr);
}

int MaxSubSum(int *arr, int left, int right)
{
	int MaxLeftSum, MaxRightSum;
	int MaxLeftBorderSum = 0, MaxRightBorderSum = 0;
	int LeftBorderSum = 0, RightBorderSum = 0;
	int i, j, Max, center;
	int flag = 0;

	if (left == right)
	{
		if (arr[left] > 0)
			return arr[left];
		else
			return 0;
	}

	center = (left + right) / 2;
	MaxLeftSum = MaxSubSum(arr, left, center);
	MaxRightSum = MaxSubSum(arr, center + 1, right);

	for (i = center; i >= left; i--)
	{
		LeftBorderSum += arr[i];
		if (LeftBorderSum > MaxLeftBorderSum)
		{
			MaxLeftBorderSum = LeftBorderSum;
			left2 = i;
			flag = 1;
		}
	}

	if (flag == 0)
		left2 = right;

	for (i = center + 1; i <= right; i++)
	{
		RightBorderSum += arr[i];
		if (RightBorderSum > MaxRightBorderSum)
		{
			MaxRightBorderSum = RightBorderSum;
			right2 = i;
		}
	}

	if (MaxLeftSum > MaxRightSum)
		Max = MaxLeftSum;
	else
		Max = MaxRightSum;

	if ((MaxLeftBorderSum + MaxRightBorderSum) > Max)
		Max = MaxLeftBorderSum + MaxRightBorderSum;

	return Max;
}

void algo3(char *file_input, char *file_output)
{
	int i, j, num, *arr;
	int ThisSum, MaxSum;
	FILE *fp1, *fp2;
	int left = 0, right = 0;

	fp1 = fopen(file_input, "rb");
	fread(&num, 4, 1, fp1);
	arr = (int*)malloc(sizeof(int)*num);
	for (i = 0; i < num; i++)
	{
		fread(&arr[i], 4, 1, fp1);
	}
	fclose(fp1);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	ThisSum = MaxSum = 0;
	for (i = 0, j = 0; j < num; j++)
	{
		ThisSum += arr[j];
		if (ThisSum > MaxSum)
		{
			MaxSum = ThisSum;
			left = i;
			right = j;
			if (j == num - 1)
				left = num - 1;
		}
		else if (ThisSum < 0)
		{
			i = j + 1;
			ThisSum = 0;
		}
	}

	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	chrono::duration<double, milli> time = std::chrono::duration_cast<chrono::duration<double, milli>>(end - start);

	if (MaxSum == 0)
	{
		left = -1;
		right = -1;
	}

	fp2 = fopen(file_output, "w");
	fprintf(fp2, "%d\n", num);
	fprintf(fp2, "3\n");
	fprintf(fp2, "%d\n", MaxSum);
	fprintf(fp2, "%d %d\n", left, right);
	fprintf(fp2, "%.4lf\n", time);
	fclose(fp2);
	free(arr);
}