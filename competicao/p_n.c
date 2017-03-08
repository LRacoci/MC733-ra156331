#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum b{false, true} bool;
#define Nat unsigned long long

void pv(const char name[], Nat *v, Nat n){
	printf("%s = [%llu",name, v[0]);
	for(unsigned i = 1; i < n; i++){
		printf(", %llu", v[i]);
	}
	printf("]\n");

}

Nat p(Nat n){
	
	bool primo;
	unsigned h[1];
	Nat *p;

	p = malloc(((n>2)?n:2)*sizeof(Nat));
	p[0] = 2;
	p[1] = 3;
	p[2] = 5;
	h[0] = 0;
	for(unsigned k = 3; k < n; k++){
		p[k] = p[k-1];
		primo = false;
		while(!primo){
			h[0] = !h[0];
			p[k] += (h[0]? 2 : 4);
			primo = true;
			for(unsigned i = 0; p[i]*p[i] <= p[k] && primo; i++){
				primo = primo && (p[k] % p[i] != 0);
			}
			//if(primo)
				//printf("%llu\n", p[k]);
		}
	}
	//pv("p", p, n);

	free(p);
}

int main(int argc, char* argv[]){
	char *endptr, *str;
	int base;

	str = argv[1];
	
	base = (argc > 2) ? atoi(argv[2]) : 10;
	
	n = strtoull(str, &endptr, base);

	printf("p(%llu)= %llu\n",n,p(n));

	
	return 0;
}