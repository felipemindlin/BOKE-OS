#include <process.h>
#include "interrupts.h"
#include <scheduler.h>
void process_wrapper(void entry_point(char ** argv), char ** argv);

int create_process(const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv){
    if(name == NULL || entry_point == NULL){
        return -1;
    }

    process_t * process = (process_t *) malloc(sizeof(process_t));
    if(process == NULL){
        return -1;
    }

    process->pid = getAvailablePid();
    
    process->name = (char *) malloc(str_len(name) + 1);
    if(process->name == NULL){
        free(process);
        return -1;
    }
    str_cpy(process->name, name);

    process->heap = (mem_block_t *) malloc(sizeof(mem_block_t));
    if(process->heap == NULL){
        free(process->name);
        free(process);
        return -1;
    }

    process->heap->base = malloc(heap_size);
    if(process->heap->base == NULL){
        free(process->heap);
        free(process->name);
        free(process);
        return -1;
    }

    process->heap->current = (uintptr_t *) process->heap->base;

    process->heap->size = heap_size;

    process->stack = (mem_block_t *) malloc(sizeof(mem_block_t));
    if(process->stack == NULL){
        free(process->heap->base);
        free(process->heap);
        free(process->name);
        free(process);
        return -1;
    }

    process->stack->base = malloc(stack_size);
    if(process->stack->base == NULL){
        free(process->stack);
        free(process->heap->base);
        free(process->heap);
        free(process->name);
        free(process);
        return -1;
    }
    
    process->stack->size = stack_size;
    
    process->stack->current = create_stackframe((uintptr_t *)entry_point, argv, process->stack->base + stack_size, &process_wrapper); // is this correct?
   
    process->status = READY;
    add_new_process(process);
    return process->pid;

}

void process_wrapper(void entry_point(char ** argv), char ** argv){
    entry_point(argv);
    // then we should kill it to allow multi-tasking. For now it is just the shell, which does not die
}

int getAvailablePid(){
    return pid++;
}

void save_reg_state(pcb_t * pcb){
	save_reg_stateAsm(&pcb->reg_state);
}

