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

//constant for number of semaphores
#define NUM_SEMS 5

int *buffer_ptr;
int sem_id, buffer_id;


void append(int x){

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 4, IPC_EXCL);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //open file
    FILE *file_ptr;
    file_ptr = fopen("output.log", "a");

    //put x in buffer and log this action
    //buffer[nextin] = x

    //sleep 1 second
    sleep(1);

    //log the time post sleep and notify that exiting

    //clean up 
    fclose(file_ptr);
    shmdt(buffer_ptr);
}

void consume(){

    int food;

    //gets shared semaphore array
	key_t sem_key = ftok("./README", 'a');
	sem_id = semget(sem_key, NUM_SEMS, 0);

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 4, IPC_EXCL);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //open file
    FILE *file_ptr;
    file_ptr = fopen("output.log", "a");

    //consume
    //food = whatever is in buffer
    //log this number and time

    //sleep for 1 second
    sleep(1);

    //do stuff (double the number in the buffer)
    //food *= 2 

    //log the new number, and state what was done. as well as record the time

    //clean up 
    fclose(file_ptr);
    shmdt(buffer_ptr);


}

void sem_wait(){


}

void sem_signal(){



}