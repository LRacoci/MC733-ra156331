#include <stdio.h>
#include <limits.h>

#include "primo.c"
int primo(int n);

int main()
{
  int n = INT_MAX;
  int i;

  for(i=0;i<=n;i++){
  	
  }

  if (primo(n))
    printf("%d é primo.\n", n);
  else
    printf("%d não é primo.\n", n);
}