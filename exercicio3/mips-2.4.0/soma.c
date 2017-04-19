#include <stdio.h>

int main(){
	int i;
	int x = 5;
	int y = 6;
	asm ("add %0, %1, %2": "=r"(x) : "r"(x), "r"(y));
	printf("Hello World !!!!1!onze!11!! %d\n", x);
	
	return 0;
}

