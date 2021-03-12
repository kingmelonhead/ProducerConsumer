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

//constants
#define NUM_SEMS 7
#define MUTEX 0
#define FREE_SPACE 1
#define IN_BUFFER 2
#define CONSUMERS_WAITING 3
#define FREE_PROC 4
#define NEXTIN 4
#define NEXTOUT 5
#define PROD_WORKING 5 
#define RUNNING_CONSUMERS 6

int *buffer_ptr;
int sem_id, buffer_id;

int log_id;
char *log_ptr;

void death_handler(){
    shmdt(buffer_ptr);
    shmdt(log_ptr);
    exit(0);
}

void produce(){

    //gets shared memory for the name of the logfile
	key_t log_key = ftok("Makefile", 'a');
	log_id = shmget(log_key, sizeof(char) * 30, IPC_EXCL);

    if (log_id == -1){
		perror("produce(): Error: Shared memory (log name) could not be created");
		printf("exiting\n\n");
        death_handler();
		exit(0);
	}

	log_ptr = (char *)shmat(log_id, 0, 0);

    //signal handlers
    signal(SIGINT, death_handler);
    srand(time(NULL));

    //wait and decrement
    sem_wait(CONSUMERS_WAITING);
    sem_signal(PROD_WORKING);
    sem_wait(FREE_SPACE);
    sem_wait(MUTEX);

    int product;
    //produce something
    product = (rand() % 513);

    printf("Produced %d\n", product);

    time_t time_start, time_end;
    struct tm * time_start_info;
    struct tm * time_end_info;

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 6, IPC_EXCL);

    if (buffer_id == -1){
		perror("produce(): Error: Shared memory (buffer) could not be created");
		printf("exiting\n\n");
        death_handler();
		exit(0);
	}

	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //open file
    FILE *file_ptr;
    file_ptr = fopen(log_ptr, "a");

    //time right before logging
    time(&time_start);
    time_start_info = localtime(&time_start);

    fprintf(file_ptr, "Writing %d to buffer space %d Time: %s", product, buffer_ptr[NEXTIN], asctime(time_start_info));

    
    //write to buffer
    buffer_ptr[buffer_ptr[NEXTIN]] = product;

    //increment head
    buffer_ptr[NEXTIN] = (buffer_ptr[NEXTIN] + 1) % 4;

    //sleep 1 second
    sleep(1);

    //log the time post sleep and notify that exiting
    time(&time_end);
    time_end_info = localtime(&time_end);
    fprintf(file_ptr, "Leaving Monitor! Time: %s \n\n", asctime(time_end_info));

    //clean up 
    fclose(file_ptr);
    shmdt(buffer_ptr);
    shmdt(log_ptr);

    //signal (increment sems)
    sem_wait(PROD_WORKING);
    sem_signal(MUTEX);
    sem_signal(IN_BUFFER);
}

void consume(){

    //gets shared memory for the name of the logfile
	key_t log_key = ftok("Makefile", 'a');
	log_id = shmget(log_key, sizeof(char) * 30, IPC_EXCL);

    if (log_id == -1){
		perror("consume(): Error: Shared memory (log name) could not be created");
		printf("exiting\n\n");
        death_handler();
		exit(0);
	}

	log_ptr = (char *)shmat(log_id, 0, 0);

    //wait and decrement
    sem_wait(IN_BUFFER);
    sem_wait(MUTEX);

    time_t time_start, time_end;
    struct tm * time_start_info;
    struct tm * time_end_info;

    int food = 0;

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 6, IPC_EXCL);

    if (buffer_id == -1){
		perror("consume(): Error: Shared memory (buffer) could not be created");
		printf("exiting\n\n");
        death_handler();
		exit(0);
	}

	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //open file
    FILE *file_ptr;
    file_ptr = fopen(log_ptr, "a");

    //time right before logging
    time(&time_start);
    time_start_info = localtime(&time_start);

    //consume
    food = buffer_ptr[buffer_ptr[NEXTOUT]];

    printf("Consumed %d\n", food);

    //log this number and time
    fprintf(file_ptr, "Got %d from the buffer at space %d Time: %s", food, buffer_ptr[NEXTOUT], asctime(time_start_info));

    //increment tail
    buffer_ptr[NEXTOUT] = (buffer_ptr[NEXTOUT] + 1) % 4;
    
    //sleep for 1 second
    sleep(1);

    //get time right before leaving monitor
    time(&time_end);
    time_end_info = localtime(&time_end);

    //log the new number, and state what was done. as well as record the time
    fprintf(file_ptr, "%d has been consumed! Time: %s\n\n", food, asctime(time_end_info));

    //clean up 
    fclose(file_ptr);
    shmdt(buffer_ptr);
    shmdt(log_ptr);

    //increment appropriate sems
    sem_signal(MUTEX);
    sem_signal(FREE_SPACE);
    

    
}

void sem_wait(int x){
    //function used for decrementing sems

    struct sembuf op;

    //gets shared semaphore array
	key_t sem_key = ftok("README", 'a');
	sem_id = semget(sem_key, NUM_SEMS, 0);

    if (sem_id == -1){
		perror("sem_wait: Error: Semaphores could not be got");
		printf("exiting\n\n");
		exit(0);
	}

    op.sem_num = x;
    op.sem_op = -1;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}

void sem_signal(int x){
    //function used for incrementing sems

    struct sembuf op;

    //gets shared semaphore array
	key_t sem_key = ftok("README", 'a');
	sem_id = semget(sem_key, NUM_SEMS, 0);

    if (sem_id == -1){
		perror("sem_signal: Error: Semaphores could not be got");
		printf("exiting\n\n");
		exit(0);
	}

    op.sem_num = x;
    op.sem_op = 1;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}