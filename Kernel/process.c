#include <process.h>
#include "interrupts.h"
#include <scheduler.h>
void process_wrapper(void entry_point(char ** argv), char ** argv);
int free_process(pcb_t * pcb);

int create_process(int parent_pid, const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv){
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
    add_new_process(process, parent_pid);
    return process->pid;

}

void process_wrapper(void entry_point(char ** argv), char ** argv){
    entry_point(argv);

    // kill_process(get_current_pcb()->process->pid);
    // printProcesses();
}

int getAvailablePid(){
    return pid++;
}

void save_reg_state(pcb_t * pcb){
	save_reg_stateAsm(&pcb->reg_state);
}

int kill_process(int pid){
    pcb_t * pcb = get_current_pcb();
    if(pcb == NULL || pcb->process == NULL){
        return -1;
    }

    pcb_t * parent_pcb = get_pcb_entry(pcb->parent_pid);
    //drawWord(parent_pcb->process->name);
    //drawNumber(parent_pcb->parent_pid);
    // if parent is dead or child is zombie, kill the child
    if( parent_pcb==NULL || parent_pcb->process->status==DEAD || pcb->process->status == ZOMBIE || pcb->parent_pid == OS_PID){
        pcb->process->status = DEAD;
        remove_process_from_scheduler(pcb);
        free_process(pcb);
    } else {
        pcb->process->status = ZOMBIE;
    }

    if(pid == get_current_pcb()->process->pid){
        // if the process is the current process, switch to the next process in the queue
        int20h();
    }

    //drawNumber(parent_pcb->process->pid);

    return 0;
}

int free_process(pcb_t * pcb){
    if(pcb == NULL || pcb->process == NULL){
        return -1;
    }

    free(pcb->process->name);
    free(pcb->process->heap->base);
    free(pcb->process->heap);
    free(pcb->process->stack->base);
    free(pcb->process->stack);
    free(pcb->process);
    free(pcb);
    
    return 0;
}
