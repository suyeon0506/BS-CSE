#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inversion_counting_problem(char *file_input, char *file_output);
void merge_sort(int *arr, int left, int right);
void merge(int *arr, int left, int middle, int right);

int *buffer; //extra space for merge sort
int count = 0;

int main(void)
{
	int i, j;
	char read_file[51], file_input[21], file_output[21];
	FILE *fp;
	fp = fopen("HW1_IC_config.txt", "r");
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
		for (; read_file[i] != '\n' && read_file[i] != '\0'; i++)
		{
			file_output[j] = read_file[i];
			j++;
		}
		file_output[j] = '\0';

		count = 0;
		inversion_counting_problem(file_input, file_output);
	}

	fclose(fp);
	return 0;
}

void inversion_counting_problem(char *file_input, char *file_output)
{
	int num, i, j;
	int *arr;
	FILE *fp1, *fp2;
	int left, right, middle;

	fp1 = fopen(file_input, "rb");
	fread(&num, 4, 1, fp1);
	arr = (int*)malloc(sizeof(int)*num);
	for (i = 0; i < num; i++)
	{
		fread(&arr[i], 4, 1, fp1);
	}
	fclose(fp1);

	buffer = (int*)malloc(sizeof(int)*(num + 1));
	left = 0;
	right = num - 1;
	merge_sort(arr, left, right);

	/*	middle = (left + right) / 2;
	i = 0;
	j = middle + 1;

	for (i = 0; i <= middle; i++)
	{
	if (arr[i] > arr[j])
	count = count + (right - j + 1);
	else
	{
	j++;
	i--;
	}
	}
	*/

	fp2 = fopen(file_output, "w");
	fprintf(fp2, "%d\n", num);
	fprintf(fp2, "%d\n", count);
	fclose(fp2);
	free(arr);
}

void merge_sort(int *arr, int left, int right)
{
	int middle;
	int i, j, num;
	num = right + 1;

	if (left < right)
	{
		middle = (left + right) / 2;

		merge_sort(arr, left, middle);
		merge_sort(arr, middle + 1, right);

		merge(arr, left, middle, right);
	}
}
void merge(int *arr, int left, int middle, int right)
{
	int i, i_left, i_right;

	memcpy(buffer + left, arr + left, sizeof(int*)*(right - left + 1));

	i_left = left;
	i_right = middle + 1;
	i = left;

	while ((i_left <= middle) && (i_right <= right))
	{
		if (buffer[i_left] < buffer[i_right])
			arr[i++] = buffer[i_left++];
		else
		{
			arr[i++] = buffer[i_right++];
			count = count + (middle - i_left + 1);
		}
	}

	while (i_left <= middle)
		arr[i++] = buffer[i_left++];
	while (i_right <= right)
		arr[i++] = buffer[i_right++];
}