#include <scheduler.h>

process_t foreground_process;
size_t processes_qty = 0;

scheduler_queue * scheduler[QUEUE_QTY];

static uint8_t isEnabled = 0;
pcb_t * current_pcb;

scheduler_queue * create_queue(int quantum);

pcb_t * get_pcb_entry(int pid){
    for(int i=0; i< QUEUE_QTY ; i++){
        node_t * current_node = scheduler[i]->queue->first;
        while(current_node != NULL){
            pcb_t * pcb = (pcb_t *) current_node->data;
            if(pcb->process->pid == pid){
                return pcb;
            }
            current_node = current_node->next;
        }
    }
    return NULL;
}

void init_scheduler(int quantum){ // for now, every queue has the same quantum. Is that correct?
    for(int i=0 ; i<QUEUE_QTY ; i++){
        scheduler[i]=create_queue(quantum);
        if(scheduler[i] == NULL){
            return;
        }
    }
}

void add_process_to_queue(process_t * process){
    // just insert nodes to the queue with priority 0 for now
    pcb_t * pcb_new = (pcb_t *) malloc(sizeof(pcb_t));
    if(pcb_new == NULL){
        return;
    }
    pcb_new->priority = 0;
    pcb_new->ticks = 0;
    pcb_new->process = process;
    // now we insert the pcb to the queue

    node_t * new_node = create_node(pcb_new);
    if(new_node == NULL){
        free(pcb_new);
        return;
    }
    // we insert the node to the queue
    insert_node(scheduler[0]->queue, new_node);
    scheduler[0]->process_qty++;
    processes_qty++;
    
}


scheduler_queue * create_queue(int quantum){
    scheduler_queue * sch_queue = (scheduler_queue *) malloc(sizeof(scheduler_queue));
    if(sch_queue == NULL){
        return NULL;
    }

    sch_queue->queue = (queue_t *) malloc(sizeof(queue_t));
    if(sch_queue->queue == NULL){
        free(sch_queue);
        return NULL;
    }

    sch_queue->quantum = quantum;
    sch_queue->queue->first = NULL;
    
    sch_queue->queue->current_node = sch_queue->queue->first;
    sch_queue->process_qty = 0;

    return sch_queue;
}

int ticks_remaining(){
    return 1;
}

int scheduler_enabled(){
    return isEnabled;
}

void enable_multitasking(int pid){
    // first, we gotta set the pcb for the first process
    current_pcb = get_pcb_entry(pid);

    isEnabled = 1;
    force_context_switch((uintptr_t *)current_pcb->process->stack->current);
}

pcb_t * get_current_pcb(){
    return current_pcb;
}

uintptr_t * switch_context(uintptr_t * current_rsp, uintptr_t * stack_segment){
    
}
