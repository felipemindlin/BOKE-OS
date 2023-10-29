#include <scheduler.h>
#include <time.h>
#include <process.h>
process_t foreground_process;
size_t processes_qty = 0;

scheduler_queue * scheduler[QUEUE_QTY];

uint8_t isEnabled = 0;
pcb_t * current_pcb;

scheduler_queue * create_queue_array(int quantum);



pcb_t * find_next_process() {
    for (int i = 0; i < QUEUE_QTY; i++) {
        if (scheduler[i]->queue->current_node) {
            pcb_t *potential_pcb = (pcb_t *)scheduler[i]->queue->current_node->data;
            if (potential_pcb->process->status != DEAD && potential_pcb->process->status != BLOCKED) {
                return potential_pcb;
            }
        }
    }
    return NULL;
}


// pcb_t * get_pcb_entry(int pid){
//     for(int i=0 ; i<QUEUE_QTY ; i++){
//         if(scheduler[i]->queue->current_node == NULL){
//             continue;
//         }
//         node_t * current_node = scheduler[i]->queue->current_node->next;
//         node_t * starting_node = scheduler[i]->queue->current_node;
//         while(current_node != NULL && current_node != starting_node){
//             pcb_t * pcb = (pcb_t *) current_node->data;
//             if(pcb->process->pid == pid){
//                 drawNumber(pcb->process->pid);
//                 while(1);
//                 return pcb;
//             }
//             current_node = current_node->next;
//         }
//     }
//     return NULL;
// }
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
    // we insert the node to the queue
    insert_node(scheduler[priority]->queue, pcb_node);
    scheduler[priority]->process_qty++;
    processes_qty++;
}


void add_new_process(process_t * process){
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
    queue->process_qty = 0;
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
    // first, we gotta set the pcb for the first process
 
    current_pcb = get_pcb_entry(pid);
    isEnabled = 1;
    drawNumber((uintptr_t *)current_pcb->process->stack->current);
    force_context_switch((uintptr_t *)current_pcb->process->stack->current);
     
    
    
}
// void enable_multitasking(process_t *process) {
//     current_pcb->process = process; 
//     force_context_switch((uintptr_t *)current_pcb->process->stack->current);
//     isEnabled = 1;
// }

pcb_t * get_current_pcb(){
    return current_pcb;
}

uintptr_t * switch_context(uintptr_t * current_rsp) {
    stop_current_process(current_rsp);
    drawWord("\n switch_context:");
    drawNumber(current_rsp);
    // if (current_pcb->process->status == BLOCKED || current_pcb->process->status == DEAD) {
    //     current_pcb = find_next_process();
    //     if (current_pcb) {
    //         current_pcb->process->status = RUNNING;
    //     }
    // }
 
    return start_next_process();
}

// uintptr_t * switch_context(uintptr_t * current_rsp){
//     stop_current_process(current_rsp);
//     // we get the next process to run

// /*  *******************************************************************************************
//     La idea está bien, acordate de poner el status del nuevo proceso (y no se si del viejo tmb)
//     Algo así ponele:

//     if(current_pcb->process->status == BLOCKED || current_pcb->process->status == DEAD){
//         // we need to get the next process to run
//         current_pcb = find_next_process();
//         current_pcb->process->status = RUNNING;
//     }

//     return start_next_process(current_pcb); // es un ejemplo nomas
//     ******************************************************************************************* */
//     return start_next_process();
// }

// void stop_current_process(uintptr_t * current_rsp){ // TO-DO NOW
    
//     current_pcb->process->stack->current = current_rsp;


//     if(current_pcb->ticks >= scheduler[current_pcb->priority]->quantum){ // if the process has used all its quantum
//         current_pcb->ticks = 0;
//         //if node is dropping in the priority range, i need to join the nodes that were joined by it (if they exist)

//         node_t * aux = scheduler[current_pcb->priority]->queue->current_node;

//         if (aux != NULL)
//         {
//             scheduler[current_pcb->priority]->queue->current_node = scheduler[current_pcb->priority]->queue->current_node->next;
//             scheduler[current_pcb->priority]->queue->current_node->prev = aux->prev;
//             aux->prev->next = aux->next; 
//             //now dec priority
//             if(current_pcb->priority < QUEUE_QTY - 1){
//                 current_pcb->priority++;
//             }
//             //now that its no longer linked to its old priority, add it to the q of its new priority
//             add_pcb_to_q(aux, current_pcb->priority);
//         }


//         // we move the current pcb to the next one
//         // current_pcb = (pcb_t *) scheduler[current_pcb->priority]->queue->current_node->data; //NOT YET
//     } else {
//         // the process has not used all its quantum, which means it has been preempted. We don't change the priority
//         // we need to send it to the "back of the queue" within its priority
//         scheduler[current_pcb->priority]->queue->current_node = scheduler[current_pcb->priority]->queue->current_node->next;
//     }

// }
void stop_current_process(uintptr_t * current_rsp) {
    current_pcb->process->stack->current = current_rsp;

    if (current_pcb->ticks >= scheduler[current_pcb->priority]->quantum) {
        current_pcb->process->status = READY;
        current_pcb->ticks = 0;

        node_t * aux = scheduler[current_pcb->priority]->queue->current_node;
       // remove_node(scheduler[current_pcb->priority]->queue, aux);
        
        if (current_pcb->priority < QUEUE_QTY - 1) {
           // current_pcb->priority++;
        }

       // add_pcb_to_q(aux, current_pcb->priority);
    } else {
        current_pcb->process->status = PREEMPTED;

        node_t * aux = scheduler[current_pcb->priority]->queue->current_node;
        remove_node(scheduler[current_pcb->priority]->queue, aux);
        add_pcb_to_q(aux, current_pcb->priority);
    }
}


// uintptr_t * start_next_process(){
//     // Snake-ISH walk through data structure. Start at highest priority and 'consume' current_node
//     // if current_node is null, drop priority and check there.

//     /* No need to walk through the same queue. If the current_node is NULL, then pq is empty. If not null, then always pick the one in front, so current_node.
//     The "walking through the same pq" part is done by the stop context function since it sets current_node to next.
//     */

//     int flag_proc_exist = 0;
//     for (priority_t priority = 0; priority < MAX_PRIORITY; priority++){
//         if (scheduler[priority]->queue->current_node == NULL){
//             continue;
//         } else {
//             current_pcb->process = (pcb_t *) scheduler[priority]->queue->current_node;
//             current_pcb->priority = priority;
//             flag_proc_exist = 1;
//         }
//     }
//     if (!flag_proc_exist)
//         return NULL;
//     return current_pcb->process->stack->current;
// }


uintptr_t * start_next_process() {
    int flag_proc_exist = 0;

    while (1){
        pcb_t * p = scheduler[0]->queue->current_node->next->data;
        drawWord(p->process->name);
        scheduler[0]->queue->current_node = scheduler[0]->queue->current_node->next;
    }

    for (priority_t priority = 0; priority < MAX_PRIORITY; priority++) {
        if (scheduler[priority]->queue->current_node) {
            current_pcb->process = (pcb_t *)scheduler[priority]->queue->current_node->data;
            current_pcb->priority = priority;
            current_pcb->process->status = RUNNING;
            flag_proc_exist = 1;
            break;
        }
    }
    if (flag_proc_exist) {
        current_pcb->process->status = RUNNING;
    }
    return current_pcb->process->stack->current;
}

int getQuantum(){
    return scheduler[current_pcb->priority]->quantum;
}