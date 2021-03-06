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

Nat p(Nat n){
	bool primo;
	unsigned h0 = 0;
	Nat *p;

	p = malloc(((n>4)?n:4)*sizeof(Nat));
	p[0] = 2;
	p[1] = 3;
	p[2] = 5;
	p[3] = 7;

	Nat pp = 1;
	for(unsigned i = 0; i < 3; i++){
		pp *= p[i];
	}
	Nat *w, w_s = 1;
	w = malloc(pp*sizeof(Nat));
	w[0] = p[3]-1;
	for(unsigned k = 4; k < pp; k++){
		p[k] = p[k-1];
		primo = false;
		for(h0 = (p[k]%6 == 1)? 0:1; !primo; h0++){
			p[k] += (h0&1 ? 2 : 4);
			primo = true;
			Nat sqrtpk = sqrt(p[k]);
			for(unsigned i = 0; p[i] <= sqrtpk && primo; i++){
				primo = primo && (p[k] % p[i] != 0);
			}
			if(primo){
				w[w_s] = p[k] - p[k-1];
				w_s++;
			}
		}
	}
	
	//#pragma omp parallel for
	for(unsigned k = 4; k < n; k++){
		p[k] = p[k-1];
		primo = false;
		for(h0 = (p[k]%6 == 1)? 0:1; !primo; h0++){
			p[k] += (h0&1 ? 2 : 4);
			primo = true;
			Nat sqrtpk = sqrt(p[k]);
			for(unsigned i = 0; p[i] <= sqrtpk && primo; i++){
				primo = primo && (p[k] % p[i] != 0);
			}
			//if(primo)
				//printf("%llu\n", p[k]);
		}
	}
	return p[n-1];
}

int main(int argc, char* argv[]){
	Nat n;
	char *endptr, *str;

	str = argv[1];
	
	n = strtoull(str, &endptr, 10);

	printf("p(%llu)= %llu\n",n,p(n));

	
	return 0;
}