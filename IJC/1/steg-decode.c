/* 
 steg-decode.c
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

#include <stdio.h>
#include <ctype.h>
#include "bitset.h"
#include "eratosthenes.c"
#include "ppm.c"
#include "error.c"


void decode(struct ppm *image){
	unsigned long size = image->xsize * image->ysize * 3;
	bitset_create(jmeno_pole, size);
	jmeno_pole[0] = size;
	eratosthenes(jmeno_pole);
	unsigned long index = 23UL;
	unsigned char c, msg = 0;
	int i = 0;
	while(42){
		if(bitset_getbit(jmeno_pole, index) == 0UL){
			c = image->data[index];
			c &= 1U;
			c <<= i;
			i++;
			msg |= c;
			if(i == 8){
				printf("%c\n", msg);
				i = 0;
				c = 255;
				msg = 0;
			}
		}
		if((msg == '\0' && i ==0 && c != 255) || index >= size){
			goto end;
		}
		index++;
	}
	end:;
}

int main(int argc, char* argv[]){
	if(argc != 2){
		error_exit("Chybny pocet argumentov.\n");
		return 1;
	}
	struct ppm *image = ppm_read(argv[1]);
	decode(image);
	ppm_free(image);
	return 0;
}