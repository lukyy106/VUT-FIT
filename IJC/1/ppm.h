/* 
 ppm.h
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

struct ppm{
	unsigned xsize;
	unsigned ysize;
	char data[]; // RGB bajty, celkom 3*xsize*ysize
};

struct ppm * ppm_read(const char * filename);

void ppm_free(struct ppm *p); 