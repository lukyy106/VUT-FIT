#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdbool.h>

#define mysleep(max_time) {if(max_time != 0) usleep((rand() % max_time)*1000); }

FILE *f;
sem_t *semafor1 = NULL; // Zabranuje ukonceniu semafora pred ukoncenim potrebnych instrukcii
sem_t *sem_imm_enter = NULL; // zabranuje koliziam pri vstupe
sem_t *sem_can_end = NULL; // podobne ako semafor1
sem_t *sem_judge = NULL; // brani koliziam
sem_t *sem_imm_leave = NULL; // zabranuje kolizi pri opustani budovy
sem_t *sem_write = NULL; // brani koliziam v zapise do suboru
sem_t *sem_judge_work = NULL; // brani koliziam imigraiv a sudcu
sem_t *sem_judge_confirm = NULL; // pocet potvrdenych imigranov
sem_t *sem_judge_is_working = NULL; // urcuje ci sa sudca nachadza v budove
int PI = 0; // pocet imigrantov
int IG = 0; // doba do noveho imigranta
int JG = 0; // doba do vstupu sudcu
int IT = 0; // doba vyzdvyhnutia certifikatu
int JT = 0; // doby rozhodnutia sudcu


int *entered; //vstupili do budovy
int *A; // line
int *NE; // pocet pritomnych nerozhodnutych
int *NC; // pocet zaregistrovanych nerozhodnutych 
int *NB; // pocet v budove