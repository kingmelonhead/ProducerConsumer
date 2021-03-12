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
#include <errno.h>

/* file - consumer.c   */

int sem_id;

void kill_handler(){
    exit(0);
}

int main(int argc, char *argv[]){

    srand(time(NULL));

    //set up handler for premature end 
    signal(SIGKILL, kill_handler);

    //get random sleep time
    int sleep_time = (rand() % (10 - 1 + 1)) + 1;

    //sleep
    sleep(sleep_time);

    consume();

    //die
    exit(0);
}