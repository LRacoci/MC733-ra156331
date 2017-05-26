#include <stdio.h>

int shared_vector[1000]
int p = 0

int main (int argc, char *argv[]){
	int i;
	while(*lock) {};

	printf("Hi from processor %i MIPS!\n", p);
	p++;
	printf("My i is @%p. Shared vector is @%p\n", &i, shared_vec)

	*lock = 0;
	while (p < 4) {}
}