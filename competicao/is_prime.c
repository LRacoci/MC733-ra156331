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


Nat mulmod_old(Nat a, Nat b, Nat mod)
{
	Nat x = 0,y = a % mod;
	while (b > 0)
	{
		if (b % 2 == 1)
		{    
			x = (x + y) % mod;
		}
		y = (y * 2) % mod;
		b /= 2;
	}
	return x % mod;
}

Nat mulmod_new(Nat x, Nat y, Nat n){
	size_t  hs = sizeof(Nat)<<2; // half_size
	// x = lx + hx*2**hs, y = ly + hy*2**hs
	//Nat lx = (x<<hs)>>hs, ux = x>>(hs), ly = (y<<hs)>>hs, uy = y>>(hs);
	Nat lx = x&0x00000000ffffffffull, ux = x>>(hs), ly = y&0x00000000ffffffffull, uy = y>>(hs);
	// lx*ly + (lx*uy + ly*ux)*2**hs + (ux*uy)*2**(2*hs)
	return ((lx*ly)%n+((((lx*uy)%n+(ly*ux)%n)%n+((ux*uy)%n*(1ull<<hs)%n)%n)*(1ull<<hs)%n)%n)%n;
}

// return (x*y)%n preventing overflow
Nat mulmod(Nat x, Nat y, Nat n){
	//Nat new = mulmod_new(x,y,n);
	Nat old = mulmod_old(x,y,n);
	/*
	if(new != old){
		printf("x = %llu\n", x);
		printf("y = %llu\n", y);
		printf("n = %llu\n", n);
		printf("# old == %llu != %llu == new\n", old, new);
	}
	*/
	return old;
}

// return (b**e)%n
Nat expmod(Nat b, Nat e, Nat n)
{
	Nat resp = 1;
	b %= n;
	while (e > 0){
		if (e % 2 == 1)
			resp = mulmod(resp, b, n);
		b = mulmod(b, b, n);
		e /= 2;
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


int main(int argc, char* argv[]){
	Nat n, *p;
	char *endptr, *str;

	str = argv[1];
	
	n = strtoull(str, &endptr, 10);

	p = pList(n);

	printf("p(%llu)= %llu\n",n,p[n-1]);

	Nat erros = 0;
	for(Nat i = 0; i < n-1; i++){
		if(!is_prime(p[i])){
			printf("%llu deu composto\n", p[i]);
			erros++;
		}
		if(is_prime(p[i]*p[i+1])){
			printf("%llu * %llu = %llu deu primo\n", p[i], p[i+1], p[i]*p[i+1]);
			erros++;
		}
	}
	printf("%llu erros\n", erros);
	
	return 0;
}