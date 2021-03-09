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

int produce(){
    //generate random number
    int x = 0;
    //x = random number 
    return x;
}

void kill_handler(){
    exit(0);
}

int get_sleep_time(){
    int x = 0;
    //generate random sleep time
    return x;
}

int main(int argc, char *argv[]){

    signal(SIGKILL, kill_handler)l

    int num;

    //gets shared semaphore array
	key_t sem_key = ftok("./README", 'a');
	sem_id = semget(sem_key, 5, IPC_CREAT | 0666);

    do {
    //produce
    num = produce();
    
    //wait for signal from monitor to append and write action to log
    //append(num);


    //sleep

    //loop till parent decides to die
    } while (1)
}