#ifndef _PROCESS_H
#define _PROCESS_H

#include <lib.h>
#include "../memory_manager/include/memory_manager.h"
#include "registers.h"
#define	__need_size_t
#define	__need_NULL
#include <stddef.h>
#define OS_PID 2
#define SHELL_PID 2
#define MAX_SEM_NAME 5
#define BASE 10
#define MIN_HEAP_SIZE 0x1000
#define MIN_STACK_SIZE 0x1000
#define MAX_PROCESS_ARGUMENTS 20

static char* status_arr[5] ={
    "BLOCKED",
    "READY",
    "RUNNING",
    "DEAD",
    "ZOMBIE"
};
enum status_t{
    BLOCKED,
    READY,
    RUNNING,
    DEAD,
    ZOMBIE
};


//#define MAX_PRIORITY 2 NO USEN ESTO. La maxima prioridad depende de QUEUE_QTY pq es el tamaño del scheduler array

typedef enum priority_t{
    IDLE_PRIORITY,
    LOW_PRIORITY,
    MEDIUM_PRIORITY,
    HIGH_PRIORITY
} priority_t;

typedef struct mem_block_t{
    void * base;
    size_t size;
    void * current;
} mem_block_t;

typedef struct process_t{
    int pid;
    int parent_pid;
    char status;
    char * name;
    char foreground;
    mem_block_t * stack;
    mem_block_t * heap;
    char sem_name[MAX_SEM_NAME];
} process_t;

typedef struct pcb_t{
    size_t ticks;
    priority_t priority;
    process_t * process;
    registerStructT reg_state;
} pcb_t;

static size_t pid = 2;

/*
    LEAVE THIS INCLUDE HERE. IT MUST BE HERE BC OF THE ORDER THE INCLUDES ARE PERFORMED.
*/
#include <scheduler.h>


process_t * create_process(int parent_pid, const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
int create_and_insert_process(int parent_pid, const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
int getAvailablePid();
void save_reg_state(pcb_t * pcb/*, registerStructT * registers*/); // COMENTO ESTO PQ ME TIRA ERROR SINO
// process_t* create_and_insert_process(const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
int kill_process(int pid);
void kill_current_process();
int kill_foreground_process(int fg_pid);
int free_process(pcb_t * pcb);
void loop();
int create_and_insert_process_from_current(const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
int waitpid(int pid);

#endif
