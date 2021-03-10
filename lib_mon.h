#define NUM_SEMS 3
#define MUTEX 0
#define FREE_SPACE 1
#define IN_BUFFER 2
#define NEXTIN 4
#define NEXTOUT 5
void produce();
void consume();
void sem_wait(int);
void sem_signal(int);