#define NUM_SEMS 6
#define MUTEX 0
#define FREE_SPACE 1
#define IN_BUFFER 2
#define CONSUMERS_WAITING 3
#define FREE_PROC 4
#define NEXTIN 4
#define NEXTOUT 5
#define PROD_WORKING 5 
void produce();
void consume();
void sem_wait(int);
void sem_signal(int);