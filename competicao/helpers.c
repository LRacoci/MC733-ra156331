#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#define Nat unsigned long long
typedef enum b{false, true} bool;

Nat divisors(Nat n){
	Nat sum = 0, i =1;
	for(i = 1; i*i <= n; i++){
		sum+= n/i - (n-1)/i;
	}
	return sum;
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
	Nat lx = (x<<hs)>>hs, ux = x>>(hs), ly = (y<<hs)>>hs, uy = y>>(hs);
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
	
int is_prime(Nat num){
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
		if(a == num) continue;
		if(a>=num) a %= num;
		if(seq == 1 || seq == num-1) continue;
		for(t = 0; t < exp; t++){
			seq = mulmod(seq,seq,num);
			if(seq == 1) return 0;
			if(seq == num-1) break;
		}
		if(t == exp) return 0;
	}
	return 1;
}
Nat pi(Nat n){
	Nat sum = 1;
	# pragma omp parallell for reduction(+:sum)
	for(Nat j = 3; j <= n; j +=2 ){
		sum += is_prime(j);
	}
	return sum;
}

Nat p(Nat n){
	Nat sum = 2, max = 2*n*log(n)+2;
	for(Nat k = 2; k <= max; k++){
		sum += (1- pi(k)/n)
	}
	return sum;
}

int main(){
	unsigned p = 0, c = 0;
	Nat num;
	while(scanf("%llu\n", &num) != EOF && num == 0){
		if (primo(num)){
			p++;
			printf("%llu é primo\n", num);
		}else{
			c++;
			printf("%llu é composto\n", num);
		}
	}
	printf("%u primos, %u compostos\n",p,c);

	return 0;
}