#include <scheduler.h>
#include <time.h>
#include <process.h>
process_t foreground_process;
size_t processes_qty = 0;

scheduler_queue * scheduler[QUEUE_QTY];

uint8_t isEnabled = 0;
pcb_t * current_pcb;

scheduler_queue * create_queue_array(int quantum);
int remove_from_queue_by_pid(queue_t * queue, int pid);
pcb_t * get_idle_pcb();

pcb_t * find_next_process( pcb_t * pcb ) {
    pcb_t * keep_running = NULL;
    for (int i = HIGH_PRIORITY; i >=LOW_PRIORITY; i--) {
        node_t * node = scheduler[i]->queue->current_node;
        while (node) {
            pcb_t *pcb = (pcb_t *)node->data;
            if (pcb->priority == DEPRECATED) {
                node = node->next;
                continue;
            }
            if (pcb->process->status == RUNNING) {
                keep_running = pcb;
            }
            if (pcb->process->status == READY) {
                pcb->process->status = RUNNING;
                scheduler[i]->queue->current_node = node->next;
                return pcb;
            }
            node = node->next;
            if (node == scheduler[i]->queue->current_node) {
                break; 
            }
        }
    }
    if (keep_running) {
        return keep_running;
    }
    return get_idle_pcb();
}

pcb_t * get_idle_pcb(){
    return (pcb_t *) scheduler[0]->queue->current_node->data;
}

pcb_t * get_pcb_entry(int pid) {
    for (int i = 0; i < QUEUE_QTY; i++) {
        node_t * current_node = scheduler[i]->queue->current_node;

        while (current_node) {
            pcb_t *pcb = (pcb_t *)current_node->data;

            if (pcb->process->pid == pid) {
                return pcb;
            }

            current_node = current_node->next;

            if (current_node == scheduler[i]->queue->current_node) {
                break; 
            }
        }
    }

    return NULL;
}


void init_scheduler(int quantum){ // for now, every queue has the same quantum. Is that correct?
    for(int i=0 ; i<QUEUE_QTY ; i++){
        scheduler[i]=create_queue_array(quantum);
        if(scheduler[i] == NULL){
            return;
        }
    }
}

void add_pcb_to_q(node_t * pcb_node, priority_t priority){
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

    node_t * new_node = create_node(pcb_new);
    if(new_node == NULL){
        free(pcb_new);
        return;
    }
    add_pcb_to_q(new_node, pcb_new->priority);
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

int ticks_remaining() {
    if(current_pcb->ticks >= scheduler[current_pcb->priority]->quantum){
        return 0;
    } else {
        current_pcb->ticks++;
        return 1;
    }
}

int scheduler_enabled(){
    return isEnabled;
}

void enable_multitasking(int pid){
 
    current_pcb = get_pcb_entry(pid);
    current_pcb->process->status = RUNNING;
    isEnabled = 1;
    force_context_switch((uintptr_t *)current_pcb->process->stack->current);
    
}

pcb_t * get_current_pcb(){
    return current_pcb;
}

uintptr_t * switch_context(uintptr_t * current_rsp) {
    current_pcb->process->stack->current = current_rsp; // save current rsp for next time
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

int remove_from_queue_by_pid(queue_t * queue, int pid) {
    node_t * current_node = queue->current_node;

    while (current_node) {
        pcb_t *pcb = (pcb_t *)current_node->data;

        if (pcb->process->pid == pid) {
            pcb->priority=DEPRECATED;
            remove_node(queue, current_node);
            return 1;
        }

        current_node = current_node->next;

        if (current_node == queue->current_node) {
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
            drawWord("-ERROR-");
        }
    }
}

void stop_current_process() {
    if (current_pcb->ticks >= scheduler[current_pcb->priority]->quantum) {        

        node_t * aux = scheduler[current_pcb->priority]->queue->current_node;
        
        if (current_pcb->priority > LOW_PRIORITY && current_pcb->priority <= HIGH_PRIORITY) {
           change_process_priority(current_pcb->process->pid, current_pcb->priority - 1);
        } else if(current_pcb->priority == LOW_PRIORITY || current_pcb->priority==IDLE_PRIORITY) {
            scheduler[LOW_PRIORITY]->queue->current_node = scheduler[LOW_PRIORITY]->queue->current_node->next;
        } else {
            while(1){
                drawWord("PROBLEMAS");
            }
        }

    } else {
/* HERE WE NEED TO CONSIDER THE CASE WHERE THE PROCESS IS BLOCKED OR DEAD. IF IT IS, WE DON'T CHANGE THE PRIORITY

        if(current_pcb->priority<QUEUE_QTY-1){
            // the new priority depends on the current priority, the time it has been running and the quantum
            int new_priority = current_pcb->priority +(int) (((double)current_pcb->ticks) / scheduler[current_pcb->priority]->quantum);
            if(new_priority > QUEUE_QTY-1){
                new_priority = QUEUE_QTY-1;
            }
            change_process_priority(current_pcb->process->pid, new_priority);
        }

        node_t * aux = scheduler[current_pcb->priority]->queue->current_node;
        remove_node(scheduler[current_pcb->priority]->queue, aux);
        add_pcb_to_q(aux, current_pcb->priority);
*/
    // FOR NOW, WE JUST HAVE NOT-ENDING PROCESSES, SO WE ENTER A WHILE(1) ERROR;
    while(1){
        drawWord("ERROR");
    }
    }
}

int getQuantum(){
    return scheduler[current_pcb->priority]->quantum;
}