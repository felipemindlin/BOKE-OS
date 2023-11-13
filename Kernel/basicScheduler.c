#include <scheduler.h>
#include <time.h>
#include <process.h>
process_t foreground_process;
size_t processes_qty = 0;

scheduler_queue * scheduler[QUEUE_QTY];
queue_t * waiting_for_remove;
queue_t * creation_queue;

uint8_t scheduler_status = 0;
pcb_t * current_pcb;

int foreground_pid = 2;

static char* status_arr[6] ={
    0,
    "BLOCKED",
    "READY",
    "RUNNING",
    "DEAD",
    "ZOMBIE"
};

scheduler_queue * create_queue_array(int quantum);
int remove_from_queue_by_pid(queue_t * queue, int pid);
pcb_t * get_idle_pcb();
void kill_processes_in_removal_queue();
void create_processes_in_creation_queue();

pcb_t * find_next_process( pcb_t * pcb ){
    pcb_t * keep_running = NULL;
    for (int i = HIGH_PRIORITY; i >=LOW_PRIORITY; i--){
        node_t * node = scheduler[i]->queue->current_node;
        while (node){
            pcb_t *pcb = (pcb_t *)node->data;
            if (pcb->process->status == ZOMBIE){
                node = node->next;
                continue;
            }
            if (pcb->process->status == RUNNING){                 keep_running = pcb;
            }
            if (pcb->process->status == READY){
                pcb->process->status = RUNNING;
                scheduler[i]->queue->current_node = node->next;
                return pcb;
            }
            node = node->next;
            if (node == scheduler[i]->queue->current_node){
                break; 
            }
        }
    }
    if (keep_running){
        return keep_running;
    }
    return get_idle_pcb();
}


pcb_t * get_idle_pcb(){
    return (pcb_t *) scheduler[0]->queue->current_node->data;
}

pcb_t * get_pcb_entry(int pid){
        for (int i = 0; i < QUEUE_QTY; i++){
        node_t * current_node = scheduler[i]->queue->current_node;

        while (current_node){
            pcb_t *pcb = (pcb_t *)current_node->data;

            if (pcb->process->pid == pid){
                return pcb;
            }

            current_node = current_node->next;

            if (current_node == scheduler[i]->queue->current_node){
                break; 
            }
        }
    }

    return NULL;
}

void init_scheduler(int quantum){
    for(int i=0 ; i<QUEUE_QTY ; i++){
        scheduler[i]=create_queue_array(1);
        if(scheduler[i] == NULL){
            return;
        }
    }
    waiting_for_remove = create_queue();
    if(waiting_for_remove == NULL){
        return;
    }
    creation_queue = create_queue();
    if(creation_queue == NULL){
        return;
    }
}

void add_pcb_to_scheduler(node_t * pcb_node, priority_t priority){
    insert_node(scheduler[priority]->queue, pcb_node);
    processes_qty++;
}


void add_new_process(process_t * process){
    pcb_t * pcb_new = (pcb_t *) malloc(sizeof(pcb_t));
    if(pcb_new == NULL){
        return;
    }
    pcb_new->priority = HIGH_PRIORITY;
    pcb_new->ticks = 0;
    pcb_new->process = process;
    
    if(pcb_new->process->pid != 1 && pcb_new->process->foreground == 1){
        foreground_pid = pcb_new->process->pid;
    }

    node_t * new_node = create_node(pcb_new);
    if(new_node == NULL){
        free(pcb_new);
        return;
    }

    add_pcb_to_scheduler(new_node, pcb_new->priority);
}

int add_process_to_removal_queue(int pid){
    node_t * new_node = create_node((void *)(uintptr_t)pid);
    if(new_node == NULL){
        return -1;
    }
    if(insert_node(waiting_for_remove, new_node)<0){
        return -1;
    }
    return waiting_for_remove->qty;
}


scheduler_queue * create_queue_array(int quantum){
    scheduler_queue * queue = (scheduler_queue *) malloc(sizeof(scheduler_queue));
    if(queue == NULL){
        return NULL;
    }
    queue->quantum = quantum;
    queue->queue = create_queue();
    if(queue->queue == NULL){
        free(queue);
        return NULL;
    }
    return queue;
}

int ticks_remaining(){
    if(current_pcb->ticks >= scheduler[current_pcb->priority]->quantum){
        return 0;
    } else {
        current_pcb->ticks++;
        return 1;
    }
}

int scheduler_enabled(){
    return scheduler_status;
}

void enable_multitasking(int pid){
 
    current_pcb = get_pcb_entry(pid);
    current_pcb->process->status = RUNNING;
    scheduler_status = 1;
    force_context_switch((uintptr_t *)current_pcb->process->stack->current);
    
}

pcb_t * get_current_pcb(){
    return current_pcb;
}



void print_process(){
        const int NAME_WIDTH = 16;
    const int STATUS_WIDTH = 12;
    const int PID_WIDTH = 8;
    const int FOREGROUND_WIDTH = 8;
    const int PRIORITY_WIDTH = 10;

        draw_word_padded(WHITE, "\nName", NAME_WIDTH);
    draw_word_padded(WHITE, "Status", STATUS_WIDTH);
    draw_word_padded(WHITE, "PID", PID_WIDTH);
    draw_word_padded(WHITE, "Fg", PID_WIDTH);
    draw_word_padded(WHITE, "Priority", PRIORITY_WIDTH);
    draw_word_padded(WHITE, "Parent PID", PID_WIDTH);
    draw_word("\n");

    for (int i = HIGH_PRIORITY; i >= 0; i--){
        node_t *node = scheduler[i]->queue->current_node;

        while (node){
            pcb_t *pcb = (pcb_t *)node->data;

            draw_word_padded(WHITE, pcb->process->name, NAME_WIDTH);
            draw_word_padded(WHITE, status_arr[(int)pcb->process->status], STATUS_WIDTH);
            draw_number_padded(WHITE, pcb->process->pid, PID_WIDTH);
            draw_number_padded(WHITE, pcb->process->foreground, FOREGROUND_WIDTH);
            draw_number_padded(WHITE, i, PRIORITY_WIDTH); 
            draw_number_padded(WHITE, pcb->process->parent_pid, PID_WIDTH);
            draw_word("\n");

                        node = node->next;

            if (node == scheduler[i]->queue->current_node){
                break;
            }
        }
    }
    draw_word("Current foreground pid: ");
    draw_number(foreground_pid);
    draw_word("\n");
}












uintptr_t * switch_context(uintptr_t * current_rsp){
    current_pcb->process->stack->current = current_rsp;             kill_processes_in_removal_queue();
    create_processes_in_creation_queue();
    stop_current_process();
    pcb_t * new_pcb = find_next_process(current_pcb);

    if(current_pcb->process->status == RUNNING ){
        current_pcb->process->status = READY;
        current_pcb->ticks = 0;
    }
                current_pcb = new_pcb;
    current_pcb->process->status = RUNNING;
    
    return current_pcb->process->stack->current;
}


int remove_from_queue_by_pid(queue_t * queue, int pid){
    node_t * current_node = queue->current_node;
    while (current_node){
        pcb_t *pcb = (pcb_t *)current_node->data;

        if (pcb->process->pid == pid){
            remove_node(queue, current_node);
            return pcb->priority;
        }

        current_node = current_node->next;

        if (current_node == queue->current_node){
            break; 
        }
    }

    return 0;
}

void change_process_priority(int pid, priority_t priority){

    pcb_t * pcb = get_pcb_entry(pid);
    if(pcb == NULL){
        return;
    }
    
    remove_from_queue_by_pid(scheduler[pcb->priority]->queue, pid);
    pcb->priority = priority;
    node_t * new_node = create_node(pcb);

    if(new_node == NULL){
        return;
    }

    if(insert_node(scheduler[priority]->queue, new_node)<0){
        while(1){
            draw_word("-ERROR-");
        }
    }
}

void stop_current_process(){
    if (current_pcb->ticks >= scheduler[current_pcb->priority]->quantum){
                  if (current_pcb->priority > LOW_PRIORITY && current_pcb->priority <= HIGH_PRIORITY){
           change_process_priority(current_pcb->process->pid, current_pcb->priority - 1);
        } 
    } else {
                        current_pcb->process->status = READY;
        current_pcb->ticks = 0;

                    }
}

int get_process_foreground_pid(){
    return foreground_pid;
}

void set_process_foreground_pid(int pid){
    foreground_pid = pid;
}

int get_quantum(){
    return scheduler[current_pcb->priority]->quantum;
}

void reassign_children_to_shell(int old_parent_pid){
    for (int i = 0; i < QUEUE_QTY; i++){
        node_t *current_node = scheduler[i]->queue->current_node;
        if (current_node == NULL){
            continue;
        }
        do {
            pcb_t *current_pcb = (pcb_t *)current_node->data;
            if (current_pcb->process->parent_pid == old_parent_pid){
                current_pcb->process->parent_pid = SHELL_PID;
            }
            current_node = current_node->next;
        } while (current_node != scheduler[i]->queue->current_node);
    }
}

void kill_processes_in_removal_queue(){
    int pid = (int) (uintptr_t) remove_node(waiting_for_remove, waiting_for_remove->current_node);
    while(pid){
        remove_from_queue_by_pid(scheduler[get_pcb_entry(pid)->priority]->queue, pid);
        free_process(get_pcb_entry(pid));
        pid = (int) (uintptr_t) remove_node(waiting_for_remove, waiting_for_remove->current_node);
    }
}



int current_process_id(){
    if(current_pcb == NULL){
        return -1;
    }
    return current_pcb->process->pid;
}

void os_revive_process(int pid){
    pcb_t *pcb = get_pcb_entry(pid);     if (pcb != NULL && (pcb->process->status == BLOCKED || pcb->process->status == RUNNING)){
        pcb->process->status = READY;             }
}


int os_block_current_process(){
    if (current_pcb == NULL){
        return -1;     }

    current_pcb->process->status = BLOCKED;                     return current_pcb->process->pid; }

int block_process(int pid){
    pcb_t *pcb = get_pcb_entry(pid);     if (pcb == NULL){
        return -1;     }
    
        if (pcb->process->status == READY || pcb->process->status == RUNNING){
        pcb->process->status = BLOCKED;                 if (pcb == current_pcb){
            finish_current_tick();
        }
        return 1;     } else if (pcb->process->status == BLOCKED){
        pcb->process->status = READY;                 return 2;     }
    
    return -1; }



int add_process_to_creation_queue(int parent_pid, uint8_t foreground, char * name, size_t stack_size, size_t heap_size, void * entry_point, void * args, int fd[2]){
    process_t * new_process = create_process(parent_pid, foreground, name, heap_size, stack_size, entry_point, args, fd[0], fd[1]);
    if (new_process == NULL){
        return -1;     }
    node_t * new_node = create_node(new_process);
    if (new_node == NULL){
        return -1;     }
    if (insert_node(creation_queue, new_node) < 0){
        return -1;     }
        return creation_queue->qty;
}

void create_processes_in_creation_queue(){
    process_t * process = (process_t *) remove_node(creation_queue, creation_queue->current_node);
    if(process == NULL){
        return;
    }
    while(process){
        add_new_process(process);
        process = (process_t *) remove_node(creation_queue, creation_queue->current_node);
    }
}

void finish_current_tick(){
    current_pcb->ticks = scheduler[current_pcb->priority]->quantum;
    force_scheduler();
}
