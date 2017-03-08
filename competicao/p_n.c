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
	unsigned h0 = 0;
	Nat *p, p_n;

	p = malloc(((n>2)?n:2)*sizeof(Nat));
	p[0] = 2;
	p[1] = 3;
	p[2] = 5;

	for(unsigned k = 3; k < n; k++){
		p[k] = p[k-1];
		#pragma omp parallell for
		for(primo = false; !primo; ){
			h0++;
			h0&=1;
			p[k] += (h0? 2 : 4);
			primo = true;
			
			#pragma omp parallell for
			for(unsigned i = 0; p[i]*p[i] <= p[k] && primo; i++){
				primo = primo && (p[k] % p[i] != 0);
			}
			//if(primo)
				//printf("%llu\n", p[k]);
		}
	}
	//pv("p", p, n);
	p_n = p[n-1];
	free(p);
	return p_n;
}

int main(int argc, char* argv[]){
	Nat n;
	char *endptr, *str;
	int base;

	str = argv[1];
	
	base = (argc > 2) ? atoi(argv[2]) : 10;
	
	n = strtoull(str, &endptr, base);

	printf("p(%llu)= %llu\n",n,p(n));

	
	return 0;
}