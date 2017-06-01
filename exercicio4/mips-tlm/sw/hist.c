#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ENDERECO_LOCK 536870912U //65

#define NUM_PROCS 4

#define MAX_BIN 256
#define MAX_H 1024
#define MAX_W 1024

#define SATURATE(min, x, max) ( ((x) < (min)) ? (min): (((x) < (max)) ? (x):(max)) )

#define CEIL_DIV(a, b) ((a)/(b) + ((a%b != 0)?1:0))


// Implementa MACROS para locks globais
#define AcquireGlobalLock() {while (*lock);}
#define ReleaseGlobalLock() {*lock = 0;}

#define SEQUENTIAL(code) {\
	AcquireGlobalLock() \
	{code} \
	ReleaseGlobalLock()\
}

// Implementa MACROS para lcoks locais
#define AcquireLock(lock) {SEQUENTIAL(while(lock); lock = 1;)} 

#define ReleaseLock(lock) {lock = 0;}

#define ATOMIC(lock, code) {\
	AcquireLock(lock) \
	{code} \
	ReleaseLock(lock)\
}

volatile int p = 0, barreira[3] = {0, 0, 0};
volatile int h, w, d;
volatile int img[MAX_H][MAX_W];
volatile int hist[NUM_PROCS+1][MAX_BIN];
volatile int *lock = (int *) ENDERECO_LOCK;
volatile int l1 = 0, l2 = 0, l3 = 0;

int main(int argc, char *argv[]){
	int i, j, my_id, my_start, my_end, magic_num;
	char c;
	char header[16];

	ATOMIC( l1,
		// Se identifica na região crítica
		my_id = p;
		// Copy this value
		p++;
		if(my_id == 0){
			scanf("%d %d\n%d", &w, &h, &d);
			//printf("%d: img.shape = (%d, %d, %d)\n",my_id,h,w,d); // Debug 0
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
		}
		
		my_start = my_id*h/NUM_PROCS;
		my_end = (1+my_id)*h/NUM_PROCS;


		//printf("%d: My vecs are:\n", my_id);
		for (i = my_start; i < my_end; i++){
			for (j = 0; j < w; j++){ 
				scanf ("%d", &img[i][j]);
				//printf(" %3d",  img[i][j]);
			}
			//printf("\n");
		}
		//printf("\n");
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
	ATOMIC( l2,
		printf("%d: Partial hist: \n", my_id);

		printf("%d: [%d", my_id, hist[my_id][0]);
		for (j = 1; j < d; j++){ 
			printf(", %d", hist[my_id][j], j);
		}
		printf("]\n");
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


	ATOMIC( l3,
		if(my_id == 0){
			printf("%d: Final hist: \n", my_id);
			printf("%d: [%d",my_id, hist[NUM_PROCS][0]);
			for (j = 1; j < d; j++){
				printf(", %d", hist[NUM_PROCS][j], j);
			}
			printf("]\n");
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

