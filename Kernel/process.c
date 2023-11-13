#include <process.h>
#include "interrupts.h"
#include <scheduler.h>
#include <libasm.h>
#include <mysemaphore.h>
#include <naiveConsole.h>

static size_t pid = 2;

int get_next_pid();
static void free_mem_zone(pcb_t * pcb);
int get_available_pid();
void process_wrapper(void entry_point(char ** argv), char ** argv);
int create_and_insert_process_from_current_standard(const char * name, uint8_t foreground, size_t *heap_and_stack, void * entry_point, void * argv, int * fd){
    return create_and_insert_process(get_current_pcb()->process->pid, foreground, name, heap_and_stack[0], heap_and_stack[1], entry_point, argv, fd[0], fd[1]);
}
int create_and_insert_process_from_current(const char * name, uint8_t foreground, size_t heap_size, size_t stack_size, void * entry_point, char ** argv, int fd[2]){
    return create_and_insert_process(get_current_pcb()->process->pid, foreground, name, heap_size, stack_size, entry_point, argv, fd[0], fd[1]);
}
int create_and_insert_process(int parent_pid, uint8_t foreground, const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv, int fr, int fw){
    if(name == NULL || entry_point == NULL){
        return -1;
    }

    process_t * process = create_process(parent_pid, foreground, name, heap_size, stack_size, entry_point, argv, fr, fw);

    if(process == NULL){
        return -1;
    }

    add_new_process(process);
    return process->pid;

}

process_t * create_process(int parent_pid, uint8_t foreground, const char * name, size_t heap_size, size_t stack_size, void * entry_point, char ** argv, int fr, int fw){

    process_t * process = (process_t *) malloc(sizeof(process_t));
    if(process == NULL){
        return NULL;
    }

    if(parent_pid < 0){
        process->parent_pid = process->pid;
    } else {
        process->parent_pid = parent_pid;
    }

    process->pid = get_available_pid();
    
    process->name = (char *) malloc((size_t)str_len(name) + 1);
    if(process->name == NULL){
        free(process);
        return NULL;
    }
    str_cpy(process->name, name);

    process->heap = (mem_block_t *) malloc(sizeof(mem_block_t));
    if(process->heap == NULL){
        free(process->name);
        free(process);
        return NULL;
    }

    if(heap_size < MIN_HEAP_SIZE){
        heap_size = MIN_HEAP_SIZE;
    }

    process->heap->base = malloc(heap_size);
    if(process->heap->base == NULL){
        free(process->heap);
        free(process->name);
        free(process);
        return NULL;
    }

    process->heap->current = (uintptr_t *) process->heap->base;

    process->heap->size = heap_size;

    process->mem_idx=0;

    process->stack = (mem_block_t *) malloc(sizeof(mem_block_t));
    if(process->stack == NULL){
        free(process->heap->base);
        free(process->heap);
        free(process->name);
        free(process);
        return NULL;
    }

    if(stack_size < MIN_STACK_SIZE){
        stack_size = MIN_STACK_SIZE;
    }

    process->stack->base = malloc(stack_size);
    if(process->stack->base == NULL){
        free(process->stack);
        free(process->heap->base);
        free(process->heap);
        free(process->name);
        free(process);
        return NULL;
    }
    
    process->stack->size = stack_size;

    process->foreground = foreground;

    if (process->foreground){
        set_process_foreground_pid(process->pid);
    }

    process->fr = fr;
    process->fw = fw;


    process->stack->current = create_stackframe((uintptr_t *)entry_point, (void*)argv, process->stack->base + stack_size, &process_wrapper);     process->status = READY;
    uint_to_base(process->pid, process->sem_name, BASE);
    my_sem_open(1, process->sem_name);
    return process;
}

void process_wrapper(void entry_point(char ** argv), char ** argv){

    entry_point(argv);

    force_kill(get_current_pcb()->process->pid);

    finish_current_tick();
}

int get_next_pid(){
    int pid_candidate = pid;
    while(get_pcb_entry(pid_candidate) != NULL){
        pid_candidate++;
    }
    return pid_candidate;
}

int get_available_pid(){
                        return pid++;
}

void save_reg_state(pcb_t * pcb){
	save_reg_state_asm(&pcb->reg_state);
}

#define SHELL_PID 2



int kill_process(int pid){
    pcb_t *pcb = get_pcb_entry(pid);
    if (pcb == NULL || pcb->process == NULL || pid==SHELL_PID){
        return -1;
    }
    free_mem_zone(pcb);
    reassign_children_to_shell(pid);

    pcb_t *parent_pcb = get_pcb_entry(pcb->process->parent_pid);
    my_sem_post(pcb->process->sem_name);

            

    if (pcb->process->parent_pid == OS_PID || parent_pcb == NULL || parent_pcb->process->status == DEAD || pcb->process->status == ZOMBIE){
        pcb->process->status = DEAD;
        add_process_to_removal_queue(pcb->process->pid);
    } else {
        pcb->process->status = ZOMBIE;
        my_sem_close(pcb->process->sem_name);
    }

    if(pid == current_process_id()){
        finish_current_tick();
    }

    return 0;
}

int kill_foreground_process(){
    int fg_pid = get_process_foreground_pid();
    if(fg_pid == SHELL_PID){
        return -1;
    }
    return force_kill(fg_pid);
}

void kill_current_process(){
    kill_process(get_current_pcb()->process->pid);
}

static void free_mem_zone(pcb_t * pcb){
    for(int i=0; i<=pcb->process->mem_idx; i++){
        if(pcb->process->mem_allocated[i]!=FREE){
            free(pcb->process->mem_allocated[i]);
        }
    }
}

int force_kill(int pid){
    pcb_t *pcb = get_pcb_entry(pid);
    if (pcb == NULL || pcb->process == NULL || pid==SHELL_PID){
        return -1;
    }
    
    if(get_process_foreground_pid() == pcb->process->pid){
        set_process_foreground_pid(SHELL_PID);
    }

    free_mem_zone(pcb);
    reassign_children_to_shell(pid);

    my_sem_post(pcb->process->sem_name);

    pcb->process->status = DEAD;
    add_process_to_removal_queue(pcb->process->pid);

    if (pid == current_process_id()){
        finish_current_tick();
    }
    return 1;
}


int free_process(pcb_t * pcb){
    if(pcb == NULL || pcb->process == NULL){
        return -1;
    }
    clear_sem(pcb->process->sem_name);
    free(pcb->process->name);
    free(pcb->process->heap->base);
    free(pcb->process->heap);
    free(pcb->process->stack->base);
    free(pcb->process->stack);
    free(pcb->process);
    free(pcb);
    
    return 0;
}

int pidd=0;
void loop(){
    if(!pid){
        pidd = create_and_insert_process(1, 1, "loop", 4096, 4096, &loop, NULL, 0, 0);
    }
    while(1){
        if(ticks_elapsed() % 100 == 0){
            draw_number(pidd);
        }
    }

}

int waitpid(int pid){
   pcb_t *pcb = get_pcb_entry(pid);

    if (pcb == NULL)
    {
        return -1;
    }
    int ret = pcb->process->pid;
    my_sem_wait(pcb->process->sem_name);

    force_kill(pcb->process->pid);
    return ret;
}
