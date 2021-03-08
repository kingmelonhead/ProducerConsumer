#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>


//variables that need to be global
pid_t *pid_list;
state *state_ptr;
int *buffer_ptr;


void display_help(){

}

void ctrl_c_handler(){

}

void kill_children(){

}

void init_pid_list(){
	int i;
	for (i=0; i<19; i++){
		pid_list[i] = 0;
	}
}

int main(int argc, char *argv[]){

	signal(SIGINT, ctrl_c_handler);

	// variable declarations

	int opt;
	int producers = 2;
	int consumers = 6; 
	int time = 100;
	int max_proc = 19;
	char log_name[20];

	//initialize the pointer to the list of pids
	pid_list = malloc(sizeof(pid_t) * max_proc);
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
				printf("logfile argument too long, defaulting to 'logfile.txt'\n\n");
			}
			else {
				strcpy(log_name, optarg);	
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

	// makes sure that # of consumers is greater than # of producers
	
	if (producers >= consumers){
		printf("Number of consumers must be greater than producers.\n");
		printf("Setting consumers to # of producers +1 \n");
		//printf("Note: if this makes producers + consumers > 20 then the values will both return to default.\n");
		//printf("This is to satisfy the maximum process limit set by the professor.\n\n");
		consumers = producers + 1;
	}


	// makes sure that the producers + consumers dont exceed the max process allowment

	if (producers + consumers > max_proc){
		printf("# of producers and consumers provided exceeds the max number of processes allowed.\n");
		printf("Defaulting to 2 producers and 6 consumers.\n\n");
		producers = 2;
		consumers = 6;
	}
	
	key_t state_key = ftok("./README", 'a');
	int state_id = shmget(state_key, sizeof(state) * (producers + consumers), IPC_CREAT | 0666);
	state_ptr = (state *)shmat(state_id, 0, 0);

	key_t buffer_key = ftok(".", 'a');
	int buffer_id = shmget(buffer_key, sizeof(int) * 4, IPC_CREAT | 0666);
	buffer_ptr = (int *)shmat(buffer_id, 0, 0);


return 0;
}
