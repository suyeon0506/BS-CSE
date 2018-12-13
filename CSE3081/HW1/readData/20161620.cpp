#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#define MAX 2000
#define dataSize 4

using namespace std;

int main() {
	FILE *fp_r;
	int idx = 0;
	int data;

	srand(time(NULL));

	//while (1) {
	int n;
	char *filename = "MSS_00.input";
	//sprintf(filename, "MSS_%02d.input", idx++);

	fp_r = fopen(filename, "rb");

	fread(&n, dataSize, 1, fp_r);
	printf("%d\n", n);

	for (int i = 0; i < n; i++) {
		fread(&data, dataSize, 1, fp_r);
		printf("%d ", data);
	}
	fclose(fp_r);
	//}

	return 0;
}