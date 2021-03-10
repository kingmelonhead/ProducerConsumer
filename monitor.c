#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include "lib_mon.h"



//variables that need to be global
pid_t *pid_list;
int *buffer_ptr;
int sem_id, buffer_id;



void detach_mem(){
	shmdt(buffer_ptr);
}

void kill_pids(){
	//function to kill all pids on the event of early termination
	int i; 
	for (i=0; i<19; i++){
		if (pid_list[i] != 0){
			kill(pid_list[i], SIGKILL);
		}
	}

}

void find_and_remove(pid_t pid){
	//finds passed pid in the global pid list and sets it to zero
	int i;
	for (i=0; i<19; i++){
		if (pid_list[i] == pid){
			pid_list[i] = 0;
			break;
		}
	}
}

void rm_mem(){
	shmctl(buffer_id, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
}

void cleanup(){
	detach_mem();
	rm_mem();
}

void display_help(){

}

void ctrl_c_handler(){
	cleanup();
	exit(0);
}

void kill_children(){

}

void init_pid_list(){
	int i;
	for (i=0; i<19; i++){
		pid_list[i] = 0;
	}
}





int main(int argc, char *argv[]){

	//signal handlers
	signal(SIGINT, ctrl_c_handler);
	signal(SIGKILL, ctrl_c_handler);
	signal(SIGALRM, ctrl_c_handler);

	// variable declarations
	int opt;
	int producers = 2;
	int consumers = 6; 
	int time = 100;
	int max_proc = 19;
	char log_name[20];

	//set alarm
	alarm(100);

	//initialize the pointer to the list of pids
	pid_list = malloc(sizeof(pid_t) * max_proc);
	init_pid_list();

	// gets options set up
	while ((opt = getopt(argc, argv, "ho:p:c:t:")) != -1){
		switch (opt) {
		case 'h':
			display_help();
			exit(0);
			break;
		case 'o':
			if (strlen(optarg) > 19){
				printf("logfile argument too long, defaulting to 'logfile.txt'\n\n");
			}
			else {
				strcpy(log_name, optarg);	
			}
			break;
		case 'p':
			producers = atoi(optarg);
			break;
		case 'c':
			consumers = atoi(optarg);
			break;
		case 't':
			if (atoi(optarg) > 100){
				printf("Time provided is too long, defaulting to 100 seconds\n\n");
			}
			else {
				time = atoi(optarg);
			}
			break;
		default:
			printf("Error: Invaliid option, calling help menu and exiting...\n\n");
			display_help();
			exit(0);
		}
	}

	// makes sure that the producers does not meet or exceed the max amount of processes allowed
	if (producers > max_proc - 1){
		printf("# of producers exceeds the max number of processes allowed\n");
		printf("Lowering to 18 producers.\n\n");
		producers = 18;
	}

	// makes sure that # of consumers is greater than # of producers
	if (producers >= consumers){
		printf("Number of consumers must be greater than producers.\n");
		printf("Setting consumers to # of producers +1 \n");
		//printf("Note: if this makes producers + consumers > 20 then the values will both return to default.\n");
		//printf("This is to satisfy the maximum process limit set by the professor.\n\n");
		consumers = producers + 1;
	}


	//gets shared semaphore array
	key_t sem_key = ftok("./README", 'a');
	sem_id = semget(sem_key, NUM_SEMS, IPC_CREAT | 0666);

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 6, IPC_CREAT | 0666);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

	//initialize the semaphores
	semctl(sem_id, MUTEX, SETVAL, 1);
	semctl(sem_id, FREE_SPACE, SETVAL, 4);
	semctl(sem_id, IN_BUFFER, SETVAL, 0);

	buffer_ptr[NEXTIN] = 0;
	buffer_ptr[NEXTOUT] = 0;

	int c;
	int p;
	pid_t pid;

	for (p = 0; p<producers; p++){
		pid = fork();
		if (pid == 0){
			execl("./producer", "./producer", (char*)0);
		}
	}
	for (c = 0; c<consumers; c++){
		pid = fork();
		if (pid == 0){
			execl("./consumer", "./consumer", (char*)0);
		}
	}

	while(1){

		
	}


	// cleanup before exiting
	cleanup();

return 0;
}
