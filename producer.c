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

/* file - producer.c */

int sem_id;

void kill_handler(){
    exit(0);
}

int main(int argc, char *argv[]){

    srand(time(NULL));

    signal(SIGKILL, kill_handler);

    int product;
    int sleep_time;

    //gets shared semaphore array
	key_t sem_key = ftok("./README", 'a');
	sem_id = semget(sem_key, NUM_SEMS, 0);

    do {
    //produce a number between 0 and 512
    product = (rand() % (513));

    //get random time to sleep
    sleep_time = (rand() % (5 - 1 + 1)) + 1;

    //sleep
    sleep(sleep_time);

    
    //wait for signal from monitor to append and write action to log
    //should be a mutex lock i think
    //append(product);


    

    //loop till parent decides to die
    //parent will die when there are no more consumers that need to eat
    } while (1);
}