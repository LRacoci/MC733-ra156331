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

volatile int p = 0, barreira[] = {0, 0, 0};
volatile int n;
volatile int inp[MAX_H][MAX_W] = {0};
volatile int out[MAX_H][MAX_W] = {0};
volatile int *lock = (int *) ENDERECO_LOCK;
volatile int l1 = 0, l2 = 0;

int main(int argc, char *argv[]){
	int i, j, k, my_id, my_start, my_end;

	//ATOMIC( l1,
	SEQUENTIAL(
		// Se identifica na região crítica
		my_id = p;
		// Incrementa pra próxima thread.
		p++;
		if(my_id == 0){
			scanf("%d \n", &n);
		}
		
		my_start = my_id*n/NUM_PROCS;
		my_end = (1+my_id)*n/NUM_PROCS;

		printf("%d: My lines:\n", my_id);
		for (i = my_start; i < my_end; i++){
			for (j = 0; j < n; j++){ 
				scanf ("%d", &inp[i][j]);
				printf(" %3d", inp[i][j]);
			}
			printf("\n");	
		}
	)

	SEQUENTIAL(
		barreira[0]++;
	)

	// Barreira para esperar printar
	while (barreira[0] < NUM_PROCS);

	for (i = my_start; i < my_end; i++){
		for (j = 0; j < n; j++){ 
			for (k = 0; k < n; k++)	{
				out[i][j] += inp[i][k] * inp[k][j];
			}
		}
	}

	SEQUENTIAL(
		barreira[1]++;
	)

	// Barreira para esperar printar
	while (barreira[1] < NUM_PROCS);
	
	//ATOMIC( l2,
	SEQUENTIAL(
		if (my_id == 2)
		for (i = 0; i < n; i++){
			for (j = 0; j < n; j++){ 
				printf(" %3d", out[i][j]);
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

