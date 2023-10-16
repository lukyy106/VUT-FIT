/* 
 bitset.h
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

#include <stdio.h>
#include <limits.h>
#include "error.h"


typedef unsigned long* bitset_t;

typedef unsigned long bitset_index_t;

#define SIZE (sizeof(unsigned long)*CHAR_BIT)

#define bitset_create(jmeno_pole, velikost) \
  unsigned long jmeno_pole[velikost / SIZE + 2]

#ifndef USE_INLINE
  #define bitset_alloc(jmeno_pole,velikost) \
      jmeno_pole = malloc(valikost / SIZE + 2)

  #define bitset_free(jmeno_pole) \
      free(jmeno_pole)

  #define bitset_size(jmeno_pole) \
      jmeno_pole[0]

  #define bitset_setbit(jmeno_pole,index,vyraz) \
      (vyraz == 0) ? (jmeno_pole[index / SIZE + 1] &= ~(1UL << (index % SIZE))) : (jmeno_pole[index / SIZE + 1] |= (1UL << (index % SIZE)))

  #define bitset_getbit(jmeno_pole,index) \
      ((jmeno_pole[index / SIZE + 1] >> (index % SIZE)) & 1UL)

#else
  inline void bitset_alloc(unsigned long *jmeno_pole, unsigned long velikost){
    jmeno_pole = malloc(valikost / SIZE + 2);
  }
  inline void bitset_free(unsigned long *jmeno_pole){
    free(jmeno_pole);
  }
  inline unsigned long bitset_size(unsigned long jmeno_pole[]){
    return jmeno_pole[0];
  }
  inline unsigned long bitset_setbit(unsigned long jmeno_pole[], unsigned long index, unsigned long vyraz){
    return (vyraz == 0) ? (jmeno_pole[index / SIZE + 1] &= ~(1UL << (index % SIZE))) : (jmeno_pole[index / SIZE + 1] |= (1UL << (index % SIZE)));
  }
  inline unsigned long bitset_getbit(unsigned long jmeno_pole[], unsigned long index){
    return ((jmeno_pole[index / SIZE + 1] >> (index % SIZE)) & 1UL);
  }
#endif
