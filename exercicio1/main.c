#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "primo.c"
int primo(int n);

int main(int argc, char* argv[])
{
  int n = atoi(argv[1]);
  int p = 0;
  int i;

  if(argc != 2){
  	return 1;
  }

  for(i = 2; i <= n; i++){
  	if(primo(i)){
  		p++;		
  	}
  }

  if(p > 0){
  	printf("Há %d primos até %d\n",p, n);
  }

  if (primo(n) && n > 1)
    printf("%d é primo.\n", n);
  else
    printf("%d não é primo.\n", n);
}