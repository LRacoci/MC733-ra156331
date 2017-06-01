#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ENDERECO_LOCK 536870912U //65

#define MAX_BIN 256
#define MAX_H 1024
#define MAX_W 1024

#define SATURATE(min, x, max) ( ((x) < (min)) ? (min): (((x) > (max)) ? (max):(x)) )


volatile int h, w, d;
volatile int img[MAX_H][MAX_W];
volatile int hist[MAX_BIN];

int main(int argc, char *argv[]){
	int i, j, my_id, my_start, my_end;

	scanf("%d %d\n%d", &w, &h, &d);
	d++;
	// Checa valores incorretos
	if(d > MAX_BIN){
		printf("d = %d > %d = MAX_BIN\n", d, MAX_BIN);
		exit(0);
	}
	if(h > MAX_H){
		printf("h = %d > %d = MAX_H\n", h, MAX_H);
		exit(0);
	}
	if(w > MAX_W){
		printf("w = %d > %d = MAX_W\n", w, MAX_W);
		exit(0);
	}
	//printf("[");
	for (i = 0; i < w; i++){
		//printf("[");
		for (j = 0; j < w; j++){ 
			scanf ("%d", &img[i][j]);
			//printf(", %3d",  img[i][j]);
		}
		//printf("\n");
	}
	//printf("\n");


	for (j = 0; j < d; j++){
		hist[j] = 0;
	}

	for (i = 0; i < h; i++){
		for (j = 0; j < w; j++){ 
			hist[SATURATE(0,img[i][j], d-1)]++;
		}
	}

	printf("Final hist: \n");
	printf("[%d", hist[0]);
	for (j = 1; j < d; j++){
		printf(", %d", hist[j], j);
	}
	printf("]\n");

	exit(0); // To avoid cross-compiler exit routine

	return 0; // Never executed, just for compatibility
}

