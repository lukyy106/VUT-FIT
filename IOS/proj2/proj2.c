
#include "proj2.h"

void imigrants(){
	
	for(int i = 1; i < PI; ++i){
		mysleep(IG);
			pid_t pid = fork();
			if(pid == 0){
				//child

				sem_wait(sem_judge);
				sem_post(sem_judge);
				sem_wait(sem_imm_enter);
				int I = i;

				sem_wait(sem_write);
				fprintf(f, "%d   : IMM %d    : starts\n",++*A , I);
				sem_post(sem_write);

				sem_wait(sem_write);
				*NE += 1;
				*NB += 1;
				fprintf(f, "%d   : IMM %d    : enters               : %d    : %d    : %d\n", ++*A, I, *NE, *NC, *NB);
				sem_post(sem_write); 

				sem_wait(sem_write);
				*NC += 1;
				fprintf(f, "%d   : IMM %d    : checks               : %d    : %d    : %d\n", ++*A, I, *NE, *NC, *NB);
				sem_post(sem_write);

				sem_post(sem_judge_work);
				
				*entered += 1;
				sem_post(sem_imm_enter);
				

				sem_wait(sem_judge);
				sem_wait(sem_judge_confirm);
				sem_wait(sem_judge_is_working);

				sem_wait(sem_write);
				fprintf(f, "%d   : IMM %d    : wants certificate    : %d    : %d    : %d\n", ++*A, I, *NE, *NC, *NB);
				sem_post(sem_write);

				sem_wait(sem_write);
				fprintf(f, "%d   : IMM %d    : got certificate      : %d    : %d    : %d\n", ++*A, I, *NE, *NC, *NB);
				sem_post(sem_write);


				sem_wait(sem_imm_leave);

				sem_wait(sem_write);
				*NB -= 1;
				fprintf(f, "%d   : IMM %d    : leaves               : %d    : %d    : %d\n", ++*A, I, *NE, *NC, *NB);
				sem_post(sem_write);

				sem_post(sem_judge_is_working);

				sem_post(sem_imm_leave);


				if(*NB == 0 && *entered == (PI-1))
					sem_post(sem_can_end);
				exit(0);
			}
			if(pid == -1){
				fprintf(stderr, "Can not fork process.\n");
				exit(1);
			}else{
				//parent	
			}
		}
	exit(0);
}




void judge(){
	while(true){
		mysleep(JG);

		sem_wait(sem_judge_work);
		sem_wait(sem_imm_enter);

		sem_wait(sem_write);
		fprintf(f, "%d   : JUDGE    : wants to enter       : %d    : %d    : %d\n", ++*A, *NE, *NC, *NB);
		sem_post(sem_write);

		sem_wait(sem_judge_is_working);

		sem_wait(sem_write);
		fprintf(f, "%d   : JUDGE    : enters               : %d    : %d    : %d\n", ++*A, *NE, *NC, *NB);
		sem_post(sem_write);
		
		sem_wait(sem_write);
		fprintf(f, "%d   : JUDGE    : starts confirmation  : %d    : %d    : %d\n", ++*A, *NE, *NC, *NB);
		sem_post(sem_write);

		mysleep(JT);

		sem_wait(sem_write);
		*NE = 0;
		while(*NC){
			sem_post(sem_judge);
			sem_post(sem_judge_confirm);
			*NC -= 1;
		}
		fprintf(f, "%d   : JUDGE    : ends confirmation    : %d    : %d    : %d\n", ++*A, *NE, *NC, *NB);
		sem_post(sem_write);
		
		sem_wait(sem_write);
		fprintf(f, "%d   : JUDGE    : leaves               : %d    : %d    : %d\n", ++*A, *NE, *NC, *NB);
		sem_post(sem_write);

		sem_post(sem_judge_is_working);

		sem_post(sem_imm_enter);

		if(*entered == (PI-1) && *NE == 0){
			sem_wait(sem_write);
			fprintf(f, "%d   : JUDGE    : finishes\n", ++*A);
			sem_post(sem_write);

			sem_post(semafor1);
			exit(0);
		}
	}
	exit(0);
}




int main(int argc, char** argv){

	if(argc < 6){
		fprintf(stderr, "Too few arguments.\n");
		exit(1);
	}
	/*
	sem_unlink("xneupa01.ios.proj2.semafor1");
	sem_unlink("xneupa01.ios.proj2.sem_imm_enter");
	sem_unlink("xneupa01.ios.proj2.sem_can_end");
	sem_unlink("xneupa01.ios.proj2.sem_judge");
	sem_unlink("xneupa01.ios.proj2.sem_imm_leave");
	sem_unlink("xneupa01.ios.proj2.sem_write");
	sem_unlink("xneupa01.ios.proj2.sem_judge_work");
	sem_unlink("xneupa01.ios.proj2.sem_judge_confirm");
	sem_unlink("xneupa01.ios.proj2.sem_judge_is_working");
	*/
	PI = atoi(argv[1]);
	PI++;
	IG = atoi(argv[2]);
	JG = atoi(argv[3]);
	IT = atoi(argv[4]);
	JT = atoi(argv[5]);

	if(IG > 2000 || JG > 2000 || IT > 2000 || JT > 2000 || IG < 0 || JG < 0 || IT < 0 || JT < 0 || PI < 2){
		fprintf(stderr, "Wrong arguments.\n");
		exit(1);
	}
	if((semafor1 = sem_open("xneupa01.ios.proj2.semafor1", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.semafor1 already exist.\n");
		exit(1);
	}
	if((sem_can_end = sem_open("xneupa01.ios.proj2.sem_can_end", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_can_end already exist.\n");
		exit(1);
	}
	if((sem_judge = sem_open("xneupa01.ios.proj2.sem_judge", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_judge already exist.\n");
		exit(1);
	}
	if((sem_imm_enter = sem_open("xneupa01.ios.proj2.sem_imm_enter", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_imm_enter already exist.\n");
		exit(1);
	}
	if((sem_imm_leave = sem_open("xneupa01.ios.proj2.sem_imm_leave", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_imm_leave already exist.\n");
		exit(1);
	}
	if((sem_write = sem_open("xneupa01.ios.proj2.sem_write", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_write already exist.\n");
		exit(1);
	}
	if((sem_judge_work = sem_open("xneupa01.ios.proj2.sem_judge_work", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_judge_work already exist.\n");
		exit(1);
	}
	if((sem_judge_confirm = sem_open("xneupa01.ios.proj2.sem_judge_confirm", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_judge_confirm already exist.\n");
		exit(1);
	}
	if((sem_judge_is_working = sem_open("xneupa01.ios.proj2.sem_judge_is_working", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
		fprintf(stderr, "Semaphore xneupa01.ios.proj2.sem_judge_is_working already exist.\n");
		exit(1);
	}
	
	entered = mmap(NULL, sizeof *entered, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	A = mmap(NULL, sizeof *A, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//line
	NE = mmap(NULL, sizeof *NE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	NC = mmap(NULL, sizeof *NC, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	NB = mmap(NULL, sizeof *NB, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	f = fopen("proj2.out", "w");
	if(f == NULL){
		fprintf(stderr, "Can not open file.\n");
		exit(1);
	}

	//setbuf(f, NULL);
	setlinebuf(f);
	pid_t pid = fork();
	if(pid == 0){
		
		pid_t pid = fork();
		if(pid == 0){
			//Immigrants
			imigrants();
		}
		if(pid == -1){
			fprintf(stderr, "Can not fork process.\n");
			exit(1);
		}else{
			//Judge
			judge();
		}
		exit(0);
	}
	if(pid == -1){
		fprintf(stderr, "Can not fork process.\n");
		exit(1);
	}
	else{
		sem_wait(semafor1);
		sem_wait(sem_can_end);

		sem_close(sem_judge_is_working);
		sem_unlink("xneupa01.ios.proj2.sem_judge_is_working");
		sem_destroy(sem_judge_is_working);

		sem_close(sem_judge_confirm);
		sem_unlink("xneupa01.ios.proj2.sem_judge_confirm");
		sem_destroy(sem_judge_confirm);

		sem_close(sem_judge_work);
		sem_unlink("xneupa01.ios.proj2.sem_judge_work");
		sem_destroy(sem_judge_work);

		sem_close(sem_write);
		sem_unlink("xneupa01.ios.proj2.sem_write");
		sem_destroy(sem_write);

		sem_close(sem_imm_leave);
		sem_unlink("xneupa01.ios.proj2.sem_imm_leave");
		sem_destroy(sem_imm_leave);

		sem_close(sem_judge);
		sem_unlink("xneupa01.ios.proj2.sem_judge");
		sem_destroy(sem_judge);

		sem_close(sem_can_end);
		sem_unlink("xneupa01.ios.proj2.sem_can_end");
		sem_destroy(sem_can_end);

		sem_close(sem_imm_enter);
		sem_unlink("xneupa01.ios.proj2.sem_imm_enter");
		sem_destroy(sem_imm_enter);

		sem_close(semafor1);
		sem_unlink("xneupa01.ios.proj2.semafor1");
		sem_destroy(semafor1);

		munmap(entered, sizeof *entered);
		munmap(A, sizeof *A);
		munmap(NE, sizeof *NE);
		munmap(NC, sizeof *NC);
		munmap(NB, sizeof *NB);
		fclose(f);
		exit(0);
	}
	return 0;
}