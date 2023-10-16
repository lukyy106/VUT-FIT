/*
Author: Lukáš Neupauer
Faculty: Faculty of Information Technology Brno University of Technology
Program: IJC - DU2
Date: 10.04.2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define LIMIT 1023 // maximum printable characters

char **constructor(int lines_count){
	char **lines = malloc(sizeof(char*) * lines_count + 1);
	if(!(lines))
		return NULL;
	for(int i = 0; i < lines_count; ++i){
		lines[i] = malloc(LIMIT+2);
		if(!(lines[i]))
			return NULL;
	}
	return lines;
}

void destructor(int lines_count, char **lines){
	for(int i = 0; i < lines_count; ++i){
		free(lines[i]);
	}
}

void sort(int lines_count, char **lines){
	char *c1;
	char *c2 = lines[lines_count];
	int i = lines_count;
	while(i > 2){
		c1 = lines[i-1];
		lines[i-1] = c2;
		c2 =  lines[i-2];
		lines[i-2] = c1;
		i -= 2;
	}
	if(i == 2){
		c1 = lines[0];
		lines[0] = c2;
		lines[lines_count] = c1;
		return;
	}
	lines[lines_count] = c2;
	return;
}

void printer(int lines_count, char **lines){
	int i = 0;
	while(lines[lines_count-1][i] != '\0')
		i++;
	if(lines[lines_count-1][i-1] != '\n' && lines[lines_count-1][i] == '\0' && i > 0)
		lines[lines_count-1][i] = '\n';
	for(i = 0; i < lines_count; ++i){
		printf("%s", lines[i]);
	}
}

void null(int lines_count, char **lines){
	for(int i = 0; i <= LIMIT; ++i){
		lines[lines_count][i] = '\0';
	}

}

int get_tail(int lines_count, const char* argv[], int n){
	char **lines = constructor(lines_count);
	if(!(lines))
		return 1;
	int c = 0, i = 0;
	lines_count--;
	if(n == 0){
		while((c = getc(stdin)) != EOF){
			if(c == '\n'){
				if(i < LIMIT)
					lines[lines_count][i] = c;
				sort(lines_count, lines);
				null(lines_count, lines);
				i = 0;
				continue;
			}
			if(i < LIMIT){
				lines[lines_count][i] = c;
			}else{
				if(i == LIMIT){
					fprintf(stderr, "Line is too long.\n");
					lines[lines_count][i] = c;
				}
			}
			i++;
		}
	}else{
		FILE *f;
		f = fopen(argv[n], "r");
		if(!(f)){
			fprintf(stderr, "Can't open file.\n");
			goto br;
		}
		while((c = getc(f)) != EOF){
			if(c == '\n'){
				if(i < LIMIT)
					lines[lines_count][i] = c;
				sort(lines_count, lines);
				null(lines_count, lines);
				i = 0;
				continue;
			}
			if(i < LIMIT){
				lines[lines_count][i] = c;
			}else{
				if(i == LIMIT){
					fprintf(stderr, "Line is too long.\n");
					lines[lines_count][i] = c;
				}
			}
			i++;
		}
		fclose(f);
	}
	br:;
	lines_count++;
	printer(lines_count, lines);
	destructor(lines_count, lines);

	return 0;
}

void print_all_stdin(int i){
	int num;
	while(i && num != EOF){
		if((num = getc(stdin)) == '\n')
			i--;
		if(i == EOF)
			exit(0);
	}
	while((i = getc(stdin)) != EOF){
		putchar(i);;
	}

}

void print_all_file(int i,  char const *argv[]){
	int num;
	FILE *f = fopen(argv[3], "r");
	if(!(f)){
		fprintf(stderr, "Can't open file.\n");
		exit(1);
	}
	while(i && num != EOF){
		if((num = getc(f)) == '\n')
			i--;
		if(i == EOF)
			exit(0);
	}
	while((i = getc(f)) != EOF){
		putchar(i);;
	}
	fclose(f);
}




int main(int argc, char const *argv[]){
	int lines_count = 10;
	int n = 0;
	if(argc > 1){
		if(argv[1][0] == '-' && argv[1][1] == 'n'){
			int lenght = strlen(argv[2]);
			if(argv[2][0] == '+'){
				for(int i = 1; i < lenght; ++i){
					if(!(isdigit(argv[2][i]))){
						fprintf(stderr, "Wrong number of lines.\n");
						return 1;
					}
				lines_count = atoi(argv[2]);
				}
				if(argc < 4){
					print_all_stdin(lines_count);
					return 0;
				}
				print_all_file(lines_count, argv);
				return 0;
			}
			
			for(int i = 0; i < lenght; ++i){
				if(!(isdigit(argv[2][i]))){
					fprintf(stderr, "Wrong number of lines.\n");
					return 1;
				}
			}
			lines_count = atoi(argv[2]);
			if(lines_count == 0)
				return 0;
			if(lines_count < 0){
				fprintf(stderr, "Wrong number of lines.\n");
				return 1;
			}
		}else{
			n = 1;
		}
		if(argc == 4)
			n = 3;
	}
	lines_count++;
	int retval = get_tail(lines_count, argv, n);
	return retval;
}