#ifndef _PROCESS_H
#define _PROCESS_H

#include <lib.h>
#include <memory_manager.h>
#include "registers.h"
#define	__need_size_t
#define	__need_NULL
#include <stddef.h>
#define OS_PID 1

enum status_t{
    BLOCKED,
    READY,
    RUNNING,
    DEAD,
    ZOMBIE
};
static char* status_arr[5] ={
    "BLOCKED",
    "READY",
    "RUNNING",
    "DEAD",
    "ZOMBIE"
};

//#define MAX_PRIORITY 2 NO USEN ESTO. La maxima prioridad depende de QUEUE_QTY pq es el tamaño del scheduler array

typedef enum priority_t{
    IDLE_PRIORITY,
    LOW_PRIORITY,
    MEDIUM_PRIORITY,
    HIGH_PRIORITY,
    DEPRECATED
} priority_t;

typedef struct mem_block_t{
    void * base;
    size_t size;
    void * current;
} mem_block_t;

typedef struct process_t{
    int pid;
    char status;
    char * name;
    mem_block_t * stack;
    mem_block_t * heap;
} process_t;

typedef struct pcb_t{
    size_t ticks;
    priority_t priority;
    process_t * process;
    int parent_pid;
    registerStructT reg_state;
} pcb_t;

static size_t pid = 2;

/*
    LEAVE THIS INCLUDE HERE. IT MUST BE HERE BC OF THE ORDER THE INCLUDES ARE PERFORMED.
*/
#include <scheduler.h>


int create_process(int parent_pid, const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
int getAvailablePid();
void save_reg_state(pcb_t * pcb/*, registerStructT * registers*/); // COMENTO ESTO PQ ME TIRA ERROR SINO
// process_t* create_process(const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
int kill_process(int pid);
int free_process(pcb_t * pcb);
void loop();

#endif
