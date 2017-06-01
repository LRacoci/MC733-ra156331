#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ENDERECO_LOCK 536870912U //64M

volatile int* lock = (int*) ENDERECO_LOCK;

int main(int argc, char *argv[]){
	*lock = 42;
	printf("Meu: %d@%p\n", *lock, lock);
	exit(0); // To avoid cross-compiler exit routine
	return 0; // Never executed, just for compatibility
}