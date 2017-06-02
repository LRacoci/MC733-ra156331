#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ENDERECO_LOCK 536870912U //64M

volatile unsigned* lock = (unsigned*) ENDERECO_LOCK;

int main(int argc, char *argv[]){
	unsigned i;
	for (i = 1; i ; i <<= 4) {
		*lock = i;
		printf("Meu:\n%x@%p\n", *lock, lock);
	}

	exit(0); // To avoid cross-compiler exit routine
	return 0; // Never executed, just for compatibility
}
