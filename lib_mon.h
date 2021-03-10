#define NUM_SEMS 3
#define MUTEX 0
#define BUFFER_SIZE 1
#define IN_BUFFER 2
void append(int);
void consume();
void sem_wait();
void sem_signal();