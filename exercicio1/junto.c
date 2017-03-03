#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <omp.h>

int primo(int n)
{
  int i;
  if(2 < n && n % 2 == 0) return 0;
  
  for(i = 3; i < n; i += 2)
    if (n % i == 0) return 0;

  return 1;
}

int main(int argc, char* argv[])
{
	int n = atoi(argv[1]);
	int p = 0;
	int i;

	if(argc != 2){
		return 1;
	}

	#pragma omp parallel for reduction (+:p)
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