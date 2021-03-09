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

int *buffer_ptr;
int sem_id, buffer_id;


void append(int x){

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 4, IPC_CREAT | 0666);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //put x in buffer

    //do stuff

    //log

    //clean up 
    shmdt(buffer_ptr);
}

void consume(){

    //gets shared semaphore array
	key_t sem_key = ftok("./README", 'a');
	sem_id = semget(sem_key, 5, IPC_CREAT | 0666);

	//gets shared memory for the buffer
	key_t buffer_key = ftok(".", 'a');
	buffer_id = shmget(buffer_key, sizeof(int) * 4, IPC_CREAT | 0666);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);

    //consume

    //do stuff

    //log

    //clean up 
    shmdt(buffer_ptr);


}

void sem_wait(){


}

void sem_signal(){



}