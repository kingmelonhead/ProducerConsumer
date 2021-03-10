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

    int sleep_time;

    do {

    produce();

    //get random time to sleep
    sleep_time = (rand() % (5 - 1 + 1)) + 1;

    //sleep
    sleep(sleep_time);

    //loop till parent decides to die
    //parent will die when there are no more consumers that need to eat
    } while (1);
}