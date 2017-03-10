#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>

#include <limits.h>
#include <errno.h>

typedef enum b{false, true} bool;
typedef unsigned long long Nat;
typedef long long double Real;


Nat *p;
unsigned tam_p = 0;
Nat *pi;

void pv(const char name[], Nat *v, Nat n){
	printf("%s = [%llu",name, v[0]);
	for(unsigned i = 1; i < n; i++){
		printf(", %llu", v[i]);
	}	
	printf("]\n");

}

void scieveTill(Nat n){
	bool primo;
	unsigned tf = 0;

	while(;tam_p < n; tam_p++){
		p[tam_p] = p[tam_p-1];
		primo = false;
		for(tf = (p[tam_p]%6 == 1)? 0:1; !primo; tf++){
			p[tam_p] += (tf&1 ? 2 : 4);
			primo = true;
			for(unsigned i = 0; p[i]*p[i] <= p[tam_p] && primo; i++){
				primo = primo && (p[tam_p] % p[i] != 0);
			}
			//if(primo)
				//printf("%llu\n", p[tam_p]);
		}
	}
}


// return (x*y)%n preventing overflow
Nat mulmod(Nat a, Nat b, Nat n){
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

Real min_p(Real n){
	Real ln = log(n);
	return n*(ln + log(ln)-1);
}
Real max_p(Real n){
	Real ln = log(n);
	return n*(ln+log(ln));
}

Nat divisors(Nat n){
	Nat sum = 0, i = 1, sq = sqrt(n);
	//#pragma omp parallel for reduction (+:sum)
	for(i = 1; i <= sq; i++){
		sum += (n/i - (n-1)/i);
	}
	return sum;
}




Real Li_inverse(Real x){
	if (x < 1)
		return 0;

	Real logx = max(1.0, log((Real) x));
	Real first = (Real) (x * logx);
	Real last  = (Real) (x * logx * 2 + 2);

	// overflow
	if (first < x) return -1;
	if (last  < x) last = prt::numeric_limits<Real>::max();

	// Find Li^-1(x) using binary search
	while (first < last)
	{
		Real mid = first + (last - first) / 2;
		if (Li(mid) < x)
			first = mid + 1;
		else
			last = mid;
	}

	return first;
}


Nat phi(Nat x, Nat a, Nat * p){
	if (a == 1){
		return (x + 1) // 2
	}
	/*
	if (phi(x, a) is in cache){
		return phi(x, a) from cache
	}
	*/
	return phi(x, a-1) - phi(x / p[a], a-1);
}

/*
//Noob way
Nat pi(Nat n){
	Nat sum = 1, sum2, j = ;
	#pragma omp parallel for reduction (+:sum)
	for(Nat j = 3; j <= n; j +=2 ){
		Nat sum2 = 0;
		sum += 1 + (2-divisors(j))/j;
	}
	return sum;
}
*/
//Neutral Way
Nat pi(Nat x){
	if(x <= tam_p){
		// pi(p[n]) == n ==> find first k s.t. p[k] <= x ==> pi(x) <= pi(p[k]) <= pi(x)
		Nat i = tam_p/2;
		while(p[i] <= x){
			i
		}

	}
	Nat a = pi(sqrt(x))
	return phi(x, a) + a - 1;
}

/*
// Very Hard Way
Nat pi(x){
	if (x < limit) return count(primes(x))
	Nat a = pi(root(x, 4)) // fourth root of x
	Nat b = pi(root(x, 2)) // square root of x
	Nat c = pi(root(x, 3)) // cube root of x
	//sum := phi(x,a) + (b+a-2) * (b-a+1) / 2
	Nat sum = phi(x,a);
	sum += ((b+a-2)*(b-a+1))>>1;
	// i from a+1 to b
	for(i = a+1, i <= b; i++){
		w = n / p[i];
		lim = pi(sqrt(w));
		sum -= pi(w);
		if (i <= c){
			//for j from i to lim
			for (j = i; j <= lim; j++){
				sum = sum - pi(w / p[j]) + j - 1
			}
		}
	} 
	return sum;
}
*/


Nat nthP(Nat n){
	Real aux = log(n);
	aux += log(aux);
	// max = x(\ln x + \ln(\ln x)-\frac{1}{2}) 
	Nat max = 6*(((Nat)(n*aux-(n>>1)))/6) + 5;
	// min = x(\ln x + \ln (\ln x)-1)
	Nat min = 6*(((Nat)(n*(aux-1)))/6) + 1;


	printf("%llu < p_%llu < %llu\n", min,n, max);

	scieveTill(min);

	Nat p_n = min;

	for(unsigned short tf = 1, p_n = min; p_n < max; p_n += (tf&1)?2:4, tf++){
		if(is_prime(p_n)){
			break;
		}
	}
	Nat i = pi(p_n);
	for(unsigned short tf = 1; p_n < max && i < n; p_n += (tf&1)?2:4, tf++){
		if(is_prime(p_n)){
			i++;
		}
	}
	if(i != n){
		printf("Chutando...\n");
	}
	return p_n;

}

int main(int argc, char* argv[]){
	Nat n;
	char *endptr, *str;

	str = argv[1];
	
	n = strtoull(str, &endptr, 10);

	p = malloc(((n>2)?n:2)*sizeof(Nat));
	p[0] = 2;
	p[1] = 3;
	p[2] = 5;

	printf("p(%llu)= %llu\n", n, nthP(n));

	
	return 0;
}