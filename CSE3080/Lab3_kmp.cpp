#include <stdio.h>
#include<string.h>
#define MAX_SIZE 100
int failure[MAX_SIZE];

int pmatch(char *string, char *pat)
{
	int i = 0, j = 0;
	int lens = strlen(string);
	int lenp = strlen(pat);

	while (i<lens && j<lenp) {
		if (string[i] == pat[j]) {
			i++; j++;
		}
		else if (j == 0)
			i++;
		else j = failure[j - 1] + 1;
	}
	return ((j == lenp) ? (i - lenp) : -1);
}

void fail (char *pat)
{
	int i, j;
	int n = strlen(pat);
	failure[0] = -1;

	for (j = 1; j<n; j++)
	{
		i = failure[j - 1];

		while ((pat[j] != pat[i + 1]) && (i >= 0))
			i = failure[i];

		if (pat[j] == pat[i + 1])
			failure[j] = i + 1;
		else
			failure[j] = -1;
	}
}

int main (void)
{
	int lens, lenp;
	char s[100], p[100];
	int result;

	FILE *fp = fopen("kmp.txt", "r");
	fscanf(fp, "%s %s", s, p);
	fclose(fp);

	lens = strlen(s);
	lenp = strlen(p);
	fail(p);
	result = pmatch(s, p);
	//fail(p);

	printf("%d\n", result);

	return 0;
}


