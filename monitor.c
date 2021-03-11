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
#include <errno.h>
#include <time.h>
#include "lib_mon.h"



//variables that need to be global
pid_t *pid_list;
int *buffer_ptr;
int sem_id, buffer_id;
int log_id;
char *log_ptr;



void detach_mem(){
	//detaches shared memory
	shmdt(buffer_ptr);
	shmdt(log_ptr);
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
	//deletes shared memory and semaphores
	shmctl(buffer_id, IPC_RMID, NULL);
	shmctl(log_id, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
}

void cleanup(){
	//function called for cleanup
	detach_mem();
	rm_mem();
}

void display_help(){
	//function to display help menu
	printf("How to invoke program: monitor [-h] [-o logfile] [-p m] [-c n] [-t time]\n\n");
	printf("This program solves the producer consumer problem using a monitor.\n");
	printf("The program has a few different options that you can use to edit how the program executes.\n");
	printf("Those options include: \n\n");
	printf( "[-h]		-This option is how the program was run to display this menu. Program outputs help menu and exits.\n\n");
	printf("[-i] logfile  -This option allows the user to change the name of the logfile. (max of 20 chars)\n\n");
	printf("[-p] m		-This option allows the user to override the default number of producers.\n\n");
	printf("[-c] n		-This option allows the user to override the default number of consumers.\n\n");
	printf("[-t] time	-This option allows the user to override the default max execution time allowed.\n\n ");
	printf("NOTE: If there is still confusion, more information can be found in the README.\n\n");
	}

void kill_children(){
	//function to kill all pids on the event of early termination
	int i; 
	for (i=0; i<19; i++){
		if (pid_list[i] != 0){
			kill(pid_list[i], SIGKILL);
		}
	}
}


void ctrl_c_handler(){
	//used to handle ctrl + c amd early exit
	fprintf(stderr, "Ctrl + c or early termination caught.");
	cleanup();
	kill_children();
	exit(0);
}


void init_pid_list(){
	//sets all pids to 0 to begin with
	int i;
	for (i=0; i<19; i++){
		pid_list[i] = 0;
	}
}

void child_handler(int sig){
	//function for handling when a child dies
	pid_t pid;
	while ((pid = waitpid((pid_t)(-1), 0, WNOHANG)) > 0) {
		find_and_remove(pid);
	}
	
}

int get_place(){
	//used to get the index of a pid in the list, used to tell the children what flag is theirs to control
	int place;
	for (place = 0; place < 19; place++){
		if (pid_list[place] == 0){
			return place;
		}
	}
}


int main(int argc, char *argv[]){

	//signal handlers
	signal(SIGINT, ctrl_c_handler);
	signal(SIGKILL, ctrl_c_handler);
	signal(SIGALRM, ctrl_c_handler);

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = child_handler;
	sigaction(SIGCHLD, &sa, NULL);

	// variable declarations
	int opt;
	int producers = 2;
	int consumers = 6; 
	int time = 100;
	int max_proc = 19;
	char log_name[25];
	char default_log[20];
	strcpy(default_log, "logfile");
	int new_log_name = 0; 
	

	//set alarm
	alarm(100);

	//initialize the pointer to the list of pids
	pid_list = malloc(sizeof(pid_t) * 19);
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
				printf("logfile argument too long, defaulting to 'logfile'\n\n");
			}
			else {
				strcpy(log_name, optarg);
				new_log_name = 1;	
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

	if (new_log_name == 0){
		strcpy(log_name, default_log);
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
		consumers = producers + 1;
	}


	//gets shared semaphore array
	key_t sem_key = ftok("./README", 'a');
	sem_id = semget(sem_key, NUM_SEMS, IPC_CREAT | 0666);

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 6, IPC_CREAT | 0666);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

	//gets shared memory for the name of the logfile
	key_t log_key = ftok("Makefile", 'a');
	log_id = shmget(log_key, sizeof(char) * 30, IPC_CREAT | 0666);
	log_ptr = (char *)shmat(log_id, 0, 0);

	sprintf(log_ptr, "%s", log_name);

	//initialize the semaphores
	semctl(sem_id, MUTEX, SETVAL, 1);
	semctl(sem_id, FREE_SPACE, SETVAL, 4);
	semctl(sem_id, IN_BUFFER, SETVAL, 0);
	semctl(sem_id, CONSUMERS_WAITING, SETVAL, consumers);
	semctl(sem_id, FREE_PROC, SETVAL, 19);

	//initialize head and tail of circular buffer
	buffer_ptr[NEXTIN] = 0;
	buffer_ptr[NEXTOUT] = 0;

	//used as temp vars and iterators 
	int c;
	int p;
	int place;

	//spawn all the producers
	for (p = 0; p<producers; p++){
		place = get_place();
		pid_list[place] = fork();
		if (pid_list[place] == 0){
			execl("./producer", "./producer", (char*)0);
		}
	}
	//spawns consumers while the number of processes is less than the max
	for (c = 0; c<consumers; c++){
		sem_wait(FREE_PROC);
		place = get_place();
		pid_list[place] = fork();
		if (pid_list[place] == 0){
			execl("./consumer", "./consumer", (char*)0);
		}
	}
	
	//waits for everything to be done
	while(1){
		if (semctl(sem_id, CONSUMERS_WAITING, GETVAL, NULL) == 0){
			while (1){
				if (semctl(sem_id, PROD_WORKING, GETVAL, NULL) == 0){
					break;
				}
			}
			break;
		}
	}


	// cleanup before exiting
	cleanup();

return 0;
}
