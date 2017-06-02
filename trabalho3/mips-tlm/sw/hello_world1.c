#include <stdio.h>
#include <stdlib.h>

#define ENDERECO_LOCK 536870912U //65

volatile int *lock = (int *) ENDERECO_LOCK;
volatile int p = 0, barreira = 0;

int main(int argc, char *argv[]){
  int i;
  // Aguarda que o valor seja 0
  while (*lock);
  // Executa algo na região crítica
  p++;
  printf("Hi from processor %d!\n", p);

  printf("ENDERECO_LOCK = %p\n", lock);

  barreira++;

  // Libera a região crítica
  *lock = 0;


  while (barreira < 2);


  exit(0); // To avoid cross-compiler exit routine
  return 0; // Never executed, just for compatibility
}

