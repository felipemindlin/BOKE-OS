#ifndef _PROCESS_H
#define _PROCESS_H

#include <lib.h>
#include <memory_manager.h>
#include "registers.h"
#define	__need_size_t
#define	__need_NULL
#include <stddef.h>
#define BLOCKED 0
#define READY 1
#define RUNNING 2
#define DEAD 3

#define MAX_PRIORITY 2

typedef enum priority_t{
    HIGH_PRIORITY,
    MEDIUM_PRIORITY,
    LOW_PRIORITY
} priority_t;

typedef enum process_status {
    READY,       // Process is ready to run
    RUNNING,     // Process is currently running
    WAITING,     // Process has finished its quantum and is waiting for its next turn
    BLOCKED,     // Process is blocked (e.g., waiting for I/O)
    PREEMPTED,   // Process was interrupted before it finished its quantum
    DEAD         // Process has finished execution
} process_status;

typedef struct mem_block_t{
    void * base;
    size_t size;
    void * current;
} mem_block_t;

typedef struct process_t{
    size_t pid;
    char status;
    char * name;
    mem_block_t * stack;
    mem_block_t * heap;
} process_t;

typedef struct pcb_t{
    size_t ticks; // ticks or quantums?
    priority_t priority;
    process_t * process;
    registerStructT reg_state;
} pcb_t;

static size_t pid = 0;

/*
    LEAVE THIS INCLUDE HERE. IT MUST BE HERE BC OF THE ORDER THE INCLUDES ARE PERFORMED.
*/
#include <scheduler.h>


int create_process(const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv);
size_t getAvailablePid();
void save_reg_state(pcb_t * pcb/*, registerStructT * registers*/); // COMENTO ESTO PQ ME TIRA ERROR SINO

#endif
