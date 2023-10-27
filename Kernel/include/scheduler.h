#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <circular_list.h>
#include <process.h>
#include <stdint.h>

#define QUEUE_QTY 3

/*
    Sugerencia para el scheduler:
        1) Scheduler que retorna siempre el mismo stack
        2) 2 procesos que hacen while(1) o un print en el while
        3) implementar una syscasll para checkear que efectivamente se bloquee
*/

typedef struct scheduler_queue {
    unsigned long quantum;
    queue_t * queue;
    unsigned int process_qty;
} scheduler_queue;

void init_scheduler(int quantum);
void terminalIsReady(int pid);
int scheduler_enabled();
int ticks_remaining();
void add_process_to_queue(process_t * process);
pcb_t * get_current_pcb();
uintptr_t * switch_context(uintptr_t * current_rsp, uintptr_t * stack_segment);

#endif