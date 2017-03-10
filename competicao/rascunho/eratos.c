#include <stdio.h>
#include <stdlib.h>

typedef enum b{false, true} bool;
#define Nat unsigned long long

void pv(Nat *v, Nat n){
	printf("{%llu", v[0]);
	for(unsigned i = 1; i < n; i++){
		printf(", %llu", v[i]);
	}
	printf("}\n");

}


int main(){
	Nat n;
	bool primo;
	
	scanf("%llu\n", &n);
	
	Nat *p;
	p = malloc(((n>2)?n:2)*sizeof(Nat));
	p[0] = 2;
	p[1] = 3;
	
	for(unsigned k = 2; k < n; k++){
		p[k] = p[k-1];
		primo = false;
		while(!primo){
			p[k] += 2;
			primo = true;
			for(unsigned i = 0; p[i]*p[i] < p[k] && primo; i++){
				primo = primo && p[k] % p[i];
			}
			//if(primo)
				//printf("%llu\n", p[k]);
		}
		//pv(p, k);
	}
	printf("p[n] = %llu\n",p[n-1]);

	free(p);
	return 0;
}