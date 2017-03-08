#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>

#include <limits.h>
#include <errno.h>

typedef enum b{false, true} bool;
#define Nat unsigned long long

void pv(const char name[], Nat *v, Nat n){
	printf("%s = [%llu",name, v[0]);
	for(unsigned i = 1; i < n; i++){
		printf(", %llu", v[i]);
	}	
	printf("]\n");
}
/*
Nat est_p(Nat n){
	Nat min = 6*((Nat)(min_p(n)/6)) + 1;
	Nat max = 6*((Nat)(max_p(n)/6)) + 5;
	for(unsigned short tf = 1, Nat p = min; p < max; p += (tf&1)?2:4, tf++){
		if(MillerRabin(p)){

		}
	}
}*/

Nat p(Nat n){
	bool primo;
	unsigned h0 = 0;
	Nat *p, p_n, sqrtpk;

	p = malloc(((n>2)?n:2)*sizeof(Nat));
	p[0] = 2;
	p[1] = 3;
	p[2] = 5;
	
	//#pragma omp parallell for
	for(unsigned k = 3; k < n; k++){
		p[k] = p[k-1];
		primo = false;
		for(h0 = (p[k]%6 == 1)? 0:1; !primo; h0++){
			p[k] += (h0&1 ? 2 : 4);
			primo = true;
			sqrtpk = sqrt(p[k]);
			for(unsigned i = 0; p[i] <= sqrtpk && primo; i++){
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

	str = argv[1];
	
	n = strtoull(str, &endptr, 10);

	printf("p(%llu)= %llu\n",n,p(n));

	
	return 0;
}