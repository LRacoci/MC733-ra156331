#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX 1024

volatile int p = 0, barreira[] = {0, 0, 0};
volatile int n;
volatile int inp[MAX][MAX] = {0};
volatile int out[MAX][MAX] = {0};

int main(int argc, char *argv[]){
	int i, j, k;

	scanf("%d \n", &n);
	
	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){ 
			scanf ("%d", &inp[i][j]);
			//printf(" %3d", inp[i][j]);
		}
		//printf("\n");	
	}


	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){ 
			for (k = 0; k < n; k++)	{
				out[i][j] += inp[i][k] * inp[k][j];
			}
		}
	}

	
	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){ 
			printf(" %3d", out[i][j]);
		}
		printf("\n");
	}

	exit(0); // To avoid cross-compiler exit routine

	return 0; // Never executed, just for compatibility
}

