#include<stdio.h>
#include<stdlib.h>	

#define WIDTH 300
#define HEIGHT 300
int n, m;
int maze[HEIGHT][WIDTH] = { 0 };

void checklink(int i, int j, int base, int dest)
{
	if (i<0)
		return;
	if (j<0)
		return;
	if (i >= m)
		return;
	if (j >= n)
		return;
	if (maze[i][j] != base)
	{
		return;
	}

	maze[i][j] = dest;
	checklink(i - 1, j, base, dest);
	checklink(i, j - 1, base, dest);
	checklink(i, j + 1, base, dest);
}//위, 아래, 양 옆이 막혀있는지 여부를 체크

int main()
{
	int i, j;
	int min, count = 0;

	int w[HEIGHT][WIDTH] = { 0 };
	int set[HEIGHT*WIDTH] = { 0 };
	int setcnt = 1;
	printf("WIDTH : ");
	scanf("%d", &n);
	printf("HEIGHT : ");
	scanf("%d", &m);
	
	for (i = 0; i<n; i++)
	{
		maze[0][i] = setcnt++;
	}

	for (i = 0; i<m - 1; i++)
	{
		for (j = 0; j<n - 1; j++)
		{
			if (maze[i][j] != maze[i][j + 1])
			{
				int isopen = rand() % 2;
				if (isopen)
				{
					checklink(i, j + 1, maze[i][j + 1], maze[i][j]);
					w[i][j] = 1;
				}
			}
		}

		for (j = 0; j<n; j++)
		{
			set[maze[i][j]] = 1;
		}

		int preset = maze[i][0];
		for (j = 0; j <= n; j++)
		{
			int isopen = rand() % 2;
			if (isopen)
			{
				maze[i + 1][j] = maze[i][j];
				set[maze[i][j]] = 0;
				w[i][j] = w[i][j] ? 3 : 2;
			}
			if (preset != maze[i][j])
			{
				if (set[preset])
				{
					maze[i + 1][j - 1] = preset;
					set[preset] = 0;
					w[i][j - 1] = w[i][j - 1] ? 3 : 2;
				}
				preset = maze[i][j];
			}
		}

		for (j = 0; j<n; j++)
		{
			if (maze[i + 1][j] == 0)
			{
				maze[i + 1][j] = setcnt++;
			}
		}
	}

	for (j = 0; j<n - 1; j++)
	{
		if (maze[m - 1][j] != maze[m - 1][j + 1])
		{
			checklink(m - 1, j + 1, maze[m - 1][j + 1], maze[m - 1][j]);
			w[m - 1][j] = 1;
		}
	}
///////////////과제를 위한 추가부분////////////////////////////////////////////////////////
	min = n;
	if (min > m)
		min = m;
	min /= 2;

	while (count!= min)
	{
		int x = rand() % (n - 1);
		int y = rand() % (m - 1);

		if (w[y][x] == 0)
		{
			int wall = rand() % 2 + 1;
			w[y][x] = wall;
			count++;
		}
		else if (w[y][x] == 1 || w[y][x] == 2)
		{
			w[y][x]++;
			count++;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////
	FILE* fp = fopen("out_maze.maz", "w");

	for (j = 0; j<n; j++)
	{
		fprintf(fp, "+-");
		printf("+-");
	}
	fprintf(fp, "+\n");
	printf("+\n");

	for (i = 0; i<m; i++)
	{
		fprintf(fp, "|");
		printf("|");
		for (j = 0; j<n; j++)
		{
			fprintf(fp, " ");
			printf(" ");
			if (w[i][j] == 1 || w[i][j] == 3)
			{
				fprintf(fp, " ");
				printf(" ");
			}
			else
			{
				fprintf(fp, "|");
				printf("|");
			}
		}
		fprintf(fp, "\n");
		printf("\n");
		for (j = 0; j<n; j++)
		{
			fprintf(fp, "+");
			printf("+");
			if (w[i][j] == 3 || w[i][j] == 2)
			{
				fprintf(fp, " ");
				printf(" ");
			}
			else
			{
				fprintf(fp, "-");
				printf("-");
			}
		}
		fprintf(fp, "+\n");
		printf("+\n");
	}


	fclose(fp);
	return 0;
}