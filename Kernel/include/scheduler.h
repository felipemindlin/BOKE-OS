#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <circular_list.h>
#include <process.h>
#include <stdint.h>
#include <mysemaphore.h>

#define QUEUE_QTY 4 

typedef struct scheduler_queue {
    unsigned int quantum;
    queue_t * queue;
} scheduler_queue;
#define SHELL_PID 2
void init_scheduler(int quantum);
void enable_multitasking(int pid);
int scheduler_enabled();
int ticks_remaining();
void add_new_process(process_t * process);
void add_pcb_to_scheduler(node_t * pcb_node, priority_t priority);
pcb_t * get_current_pcb();
pcb_t * find_next_process();
uintptr_t * switch_context(uintptr_t * current_rsp);
void stop_current_process();
int get_process_foreground_pid();
void set_process_foreground_pid(int pid);
int get_quantum();
pcb_t * get_pcb_entry(int pid);
int add_process_to_removal_queue(int pid);
int add_process_to_creation_queue(int parent_pid, uint8_t foreground, char * name, size_t stack_size, size_t heap_size, void * entry_point, void * args, int fd[2]);
void print_process();
int current_process_id();
void change_process_priority(int pid, priority_t priority);
int block_process(int pid);
void finish_current_tick();
void reassign_children_to_shell(int old_parent_pid);
#endif