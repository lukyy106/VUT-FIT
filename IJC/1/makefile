# makefile
# Autor: Lukáš Neupauer xneupa01
# Project: IJC DU1

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic  -g 

all: primes steg-decode primes-i

primes: primes.c eratosthenes.c bitset.h
	$(CC) $(CFLAGS) primes.c -o primes -lm

primes-i: primes.c eratosthenes.c bitset.h
	$(CC) $(CFLAGS) -DUSE_ILINE primes.c -o primes-i -lm

steg-decode: steg-decode.c eratosthenes.c ppm.c error.c bitset.h
	$(CC) $(CFLAGS) steg-decode.c -o steg-decode -lm

clean:
	rm  steg-decode primes primes-i *.o