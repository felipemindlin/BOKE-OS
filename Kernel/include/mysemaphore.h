#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>

#define MAX_IDENTIFIER 15
#define MAX_SEMS 128
#define MAX_PROCESSES_PER_SEM 10

#define AVAILABLE 1
#define UNAVAILABLE 0

#include <circular_list.h>

typedef struct {
    char identifier[MAX_IDENTIFIER];
    node_t *head;  // first process waiting in queue
    node_t *tail;  // last process waiting in queue
    uint64_t is_locked;
    int counter;
    uint64_t users_count;    // number of processes using the semaphore
    uint64_t queue_size;     // number of blocked processes
    int allowed_processes[MAX_PROCESSES_PER_SEM];  // List of PIDs of processes allowed to use the semaphore
    int allowed_process_count;  // The number of processes currently allowed to use the semaphore
    int being_cleared;
} mySem_t;

void initialize_sems();
uint64_t my_sem_open(uint64_t start_value, char *id);
void my_sem_close(char *id);
uint64_t my_sem_post(char *id); 
uint64_t my_sem_wait(char *id);  
int locate_sem(char *id);
int add_to_queue(int sem_idx, int pid);
int remove_from_queue(int sem_idx);
int get_sem(char *id, int start_value);
int put_sem(char *id);
void whiff(uint64_t sem_idx);
int clear_sem(char *sem_id);

#endif
