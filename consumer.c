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

/* file - consumer.c   */

void kill_handler(){

    exit(0);
}

int main(int argc, char *argv[]){

    signal(SIGKILL, kill_handler);

    //wait for signal to consume
    
    //consume and log in monitor 

    //die

}