#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <circular_list.h>
#include <process.h>
#include <stdint.h>

#define QUEUE_QTY 3 // Depends on the number of priorities we want to have

/*
    Sugerencia para el scheduler:
        1) Scheduler que retorna siempre el mismo stack
        2) 2 procesos que hacen while(1) o un print en el while
        3) implementar una syscasll para checkear que efectivamente se bloquee
*/

typedef struct scheduler_queue {
    unsigned int quantum;
    queue_t * queue;
    unsigned int process_qty;
} scheduler_queue;

void init_scheduler(int quantum);
void enable_multitasking(int pid);
int scheduler_enabled();
int ticks_remaining();
void add_new_process(process_t * process);
void add_pcb_to_q(node_t * pcb_node, priority_t priority);
pcb_t * get_current_pcb();
uintptr_t * switch_context(uintptr_t * current_rsp);
uintptr_t * start_next_process();
#endif