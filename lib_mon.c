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
#define NUM_SEMS 3
#define MUTEX 0
#define BUFFER_SIZE 1
#define IN_BUFFER 2

int *buffer_ptr;
int sem_id, buffer_id;


void append(int x){

    time_t time_start, time_end;
    struct tm * time_start_info;
    struct tm * time_end_info;

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 4, IPC_EXCL);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //open file
    FILE *file_ptr;
    file_ptr = fopen("output.log", "a");

    time(&time_start);
    time_start_info = localtime(&time_start);

    fprintf(file_ptr, "Writing %d to buffer and sleeping for 1 second. Time: %s \n", x, asctime(time_start_info));

    //put x in buffer and log this action
    //buffer[nextin] = x

    //sleep 1 second
    sleep(1);

    //log the time post sleep and notify that exiting
    time(&time_end);
    time_end_info = localtime(&time_end);
    fprintf(file_ptr, "Writing and sleeping done, ending now. Time %s \n\n", asctime(time_end_info));

    //clean up 
    fclose(file_ptr);
    shmdt(buffer_ptr);
}

void consume(){

    time_t time_start, time_end;
    struct tm * time_start_info;
    struct tm * time_end_info;

    int food = 0;

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

    time(&time_start);
    time_start_info = localtime(&time_start);
    //consume
    //food = whatever is in buffer
    //log this number and time
    fprintf(file_ptr, "Got %d from the buffer. This will be consumed after sleeping for 1 second (doubled, logged, then deleted from buffer). Time: %s", food, asctime(time_start_info));
    //sleep for 1 second
    sleep(1);

    //do stuff (double the number in the buffer)
    food *= 2;

    time(&time_end);
    time_end_info = localtime(&time_end);
    //log the new number, and state what was done. as well as record the time
    fprintf(file_ptr, "The product in the buffer, after being doubled is %d. Time: %s  ...now ending\n\n", food, asctime(time_end_info));
    //clean up 
    fclose(file_ptr);
    shmdt(buffer_ptr);


}

void sem_wait(){


}

void sem_signal(){



}