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

Nat* pList(Nat n){
	bool primo;
	unsigned h0 = 0;
	Nat *p;

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
			for(unsigned i = 0; p[i]*p[i] <= p[k] && primo; i++){
				primo = primo && (p[k] % p[i] != 0);
			}
			//if(primo)
				//printf("%llu\n", p[k]);
		}
	}
	return p;
}


// return (x*y)%n preventing overflow
Nat mulmod(Nat x, Nat y, Nat n){
	Nat x = 0, y = a % n;
	while (b){
		if (b & 1)    
			x = (x + y) % n;
		y = (y << 1) %n; 
		b >>= 1;
	}return x % n;
}

// return (b**e)%n
Nat expmod(Nat b, Nat e, Nat n)
{
	Nat resp = 1;
	b %= n;
	while (e){
		if (e & 1)
			resp = mulmod(resp, b, n);
		b = mulmod(b, b, n);
		e >>= 1;
	}
	return resp % n;
}
	
bool is_prime(Nat num){
		// Base encontrada por Jim Sinclair
		Nat as[12] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
		int t;
	
	int exp = 0;
	if (num < 2) return 0;
	if (num == 2) return 1;
	if (num % 2 == 0) return 0; 
	// Encontra exp de forma que num-1 = odd*2**exp com odd impar
	Nat odd = num - 1; 
	while ((odd & 1) == 0){
		exp++;
		odd >>= 1;
	}
	// exp == 0 ==> odd == num-1 eh par ==> Absurdo

	/* Contrapositiva do teorema de Miller-Rabin:
		Se existe a < num, tal que: 
			1. a**odd % num != 1
			2. Para qualquer t < exp : a**(odd*2**t)%num != num-1
		Então:
			num é composto
		*/
	for (int i = 0; i < 12; i++){
		Nat a = as[i];
		Nat seq = expmod(a,odd,num); 
		if(a == num) 
			continue;
		if(a>=num) 
			a %= num;
		else if(seq == num-1) 
			continue;
		for(t = 0; t < exp && seq != 1 && seq != num-1; t++){
			seq = mulmod(seq,seq,num);
		}
		if(seq != num-1 && t){
			return false;
		}
	}
	return true;
}

Nat p(Nat n){
	Nat min = 6*((Nat)(min_p(n)/6)) + 1;
	Nat max = 6*((Nat)(max_p(n)/6)) + 5;
	for(unsigned short tf = 1, Nat p = min; p < max; p += (tf&1)?2:4, tf++){
		if(is_prime(p)){
			
		}
	}
}

Nat p(Nat n){

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