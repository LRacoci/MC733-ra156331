#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ENDERECO_LOCK 536870912U //65

#define NUM_PROCS 4

#define MAX_BIN 256
#define MAX_IMG 2048

#define SATURATE(min, x, max) ( ((x) < (min)) ? (min): (((x) < (max)) ? (x):(max)) )

#define CEIL_DIV(a, b) ((a)/(b) + ((a%b != 0)?1:0))



#define AcquireGlobalLock() {while (*lock);}
#define ReleaseGlobalLock() {*lock = 0;}

#define SEQUENTIAL(code) {\
	AcquireGlobalLock() \
	{code} \
	ReleaseGlobalLock()\
}

#define AcquireLock(lock) {SEQUENTIAL(while(lock); lock = 1;)} 

#define ReleaseLock(lock) {lock = 0;}

volatile int *lock = (int *) ENDERECO_LOCK;
volatile int p = 0, barreira[3] = {0, 0, 0};
volatile int h, w, d;
volatile int img[MAX_IMG][MAX_IMG];
volatile int hist[NUM_PROCS+1][MAX_BIN];

int main(int argc, char *argv[]){
	int i, j, my_id, my_start, my_end;

	SEQUENTIAL(
		// Se identifica na região crítica
		my_id = p;
		//Copy this value
		p++;
		if(my_id == 0){
			//Le entrada
			scanf("%d %d %d", &h, &w, &d);
			//printf("%d: img.shape = (%d, %d, %d)\n",my_id,h,w,d); // Debug 0
			// Checa valores incorretos
			if(d >= MAX_BIN){
				printf("d = %d >= %d = MAX_BIN\n", d, MAX_BIN);
				exit(0);
			}
			if(w >= MAX_IMG){
				printf("w = %d >= %d = MAX_IMG\n", w, MAX_IMG);
				exit(0);
			}
			if(h >= MAX_IMG){
				printf("h = %d >= %d = MAX_IMG\n", h, MAX_IMG);
				exit(0);
			}
		}
		
		my_start = my_id*h/NUM_PROCS;
		my_end = (1+my_id)*h/NUM_PROCS;


		printf("%d: My vecs are:\n", my_id);
		/*
		*/
		for (i = my_start; i < my_end; i++){
			for (j = 0; j < w; j++){ 
				scanf ("%d", &img[i][j]);
				printf(" %3d",  img[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	)



	// Calcula os histogramas parciais

	for (j = 0; j < d; j++){
		hist[my_id][j] = 0;
	}

	for (i = my_start; i < my_end; i++){
		for (j = 0; j < w; j++){ 
			hist[my_id][SATURATE(0,img[i][j], d-1)]++;
		}
	}
	

	
	SEQUENTIAL(
		barreira[0]++;
	)

	// Barreira para separar as duas partes
	while (barreira[0] < NUM_PROCS);
	/////////////////////////////////////////////////////////////////////////////


	// Imprime os histogramas paciais obtidos até agora
	SEQUENTIAL(
		printf("%d: Partial hist: \n", my_id);

		printf("%d: %3d[0]", my_id, hist[my_id][0]);
		for (j = 1; j < d; j++){ 
			printf(" %3d[%d]", hist[my_id][j], j);
		}
		printf("\n");
	)

	// Agora somaremos os histogramas parciais

	for (i = my_id; i < d; i+=NUM_PROCS){
		hist[NUM_PROCS][i] = 0;
		for (j = 0; j < NUM_PROCS; j++){
			hist[NUM_PROCS][i] += hist[j][i];
		}
	}

	SEQUENTIAL(
		barreira[1]++;
	)

	// Barreira para esperar todo mundo acabar de somar
	while (barreira[1] < NUM_PROCS);
	/////////////////////////////////////////////////////////////////////////////
	// Agora só falta imprimir a resposta final


	SEQUENTIAL(
		if(my_id == 0){
			printf("%d: Final hist: \n", my_id);
			printf("%d: %3d[0]",my_id, hist[NUM_PROCS][0]);
			for (j = 1; j < d; j++){
				printf(" %3d[%d]", hist[NUM_PROCS][j], j);
			}
			printf("\n");
		}
		
	)

	SEQUENTIAL(
		barreira[2]++;
	)

	// Barreira para esperar printar
	while (barreira[2] < NUM_PROCS);


	exit(0); // To avoid cross-compiler exit routine

	return 0; // Never executed, just for compatibility
}

