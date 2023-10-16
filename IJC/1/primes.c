/* 
 primes.c
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

#define N 500000000L
#include <stdio.h>
#include "bitset.h"
#include "eratosthenes.c"


void write(bitset_t jmeno_pole){
	int j = 0;
	unsigned long primes_10 [10];
	for(unsigned long i = N; j < 10 && i !=1 ;i--){
		if(bitset_getbit(jmeno_pole, i) == 0){
			primes_10[j] = i;
			j++;
		}
	}
	for (int i = 9; i >=0; i--){
		printf("%lu\n", primes_10[i]);
	}
}


int main(){
	bitset_create(jmeno_pole, N);
	jmeno_pole[0] = N;
	eratosthenes(jmeno_pole);
	write(jmeno_pole);
	return 0;
}