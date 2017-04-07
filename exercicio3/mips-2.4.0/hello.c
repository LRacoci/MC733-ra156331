#include <stdio.h>

int main(){
	int i;
	int x = 2;
	int y = 3;
	printf("Hello World !!!!1!!11!!onze %d\n", x + y);
	for (i = 0; i < 1000; i++){
		printf("%d\n", i+x);
		asm ("add %0, %1, %2": "=r"(x) : "r"(i), "r"(x));
	}
	return 0;
}

