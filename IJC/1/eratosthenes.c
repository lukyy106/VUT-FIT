/* 
 eratosthenes.c
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

#include <stdio.h>
#include <math.h>
#include "eratosthenes.h"


void eratosthenes(bitset_t jmeno_pole){
	unsigned long n = bitset_size(jmeno_pole);
	unsigned long limit = n;
	limit = sqrt(n) + 1;
	bitset_setbit(jmeno_pole, 0, 1);
	bitset_setbit(jmeno_pole, 1, 1);
	for(unsigned long i = 2; i < limit; i++){
		if(bitset_getbit(jmeno_pole, i) == 0){
			for(unsigned long j = i; j*i <= n; j++){
				bitset_setbit(jmeno_pole, j*i, 1);
			}
		}
	}
}