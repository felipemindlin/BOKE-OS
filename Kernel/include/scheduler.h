#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <circular_list.h>
#include <process.h>
#include <stdint.h>

#define QUEUE_QTY 4 // Depends on the number of priorities we want to have

/*
    Sugerencia para el scheduler:
        1) Scheduler que retorna siempre el mismo stack
        2) 2 procesos que hacen while(1) o un print en el while
        3) implementar una syscasll para checkear que efectivamente se bloquee
*/

typedef struct scheduler_queue {
    unsigned int quantum;
    queue_t * queue;
} scheduler_queue;

void init_scheduler(int quantum);
void enable_multitasking(int pid);
int scheduler_enabled();
int ticks_remaining();
void add_new_process(process_t * process, int parent_pid);
void add_pcb_to_q(node_t * pcb_node, priority_t priority);
pcb_t * get_current_pcb();
pcb_t * find_next_process();
uintptr_t * switch_context(uintptr_t * current_rsp);
void stop_current_process();
int getQuantum();
pcb_t * get_pcb_entry(int pid);
int add_process_to_removal_queue(int pid);
void print_process();
int current_process_id();
void change_process_priority(int pid, priority_t priority);
#endif