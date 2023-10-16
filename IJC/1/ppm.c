/* 
 ppm.c
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

#include <stdlib.h>
#include <stdio.h>
#include "ppm.h"

struct ppm * ppm_read(const char * filename){
	FILE *f = fopen(filename, "r");
	if(f == NULL){
		warning_msg("Subor %s sa nepodarilo otvorit.\n", filename);
		return NULL;
	}
	unsigned xsize, ysize;
	if(fscanf(f, "P6 %u %u 255", &xsize, &ysize) != 2){
		fclose(f);
		warning_msg("subor %s nie je validny.\n", filename);
		return NULL;
	}
	unsigned long size = xsize * ysize * 3 *sizeof(char);
	struct ppm *image = malloc(size + sizeof(struct ppm));
	if(image == NULL){
		fclose(f);
		warning_msg("Alokacia sa nepodarila.\n");
		return NULL;
	}
	if(fread(&image->data, sizeof(char), size, f) != size || ferror(f) != 0 /*|| fgetc(f) != EOF*/){
		fclose(f);
		ppm_free(image);
		warning_msg("Obrazok nie je validny.\n");
		return NULL;
	}
	fclose(f);
	image->xsize = xsize;
	image->ysize = ysize;
	return image;
}

void ppm_free(struct ppm *p){
	free(p);
}