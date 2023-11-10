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
//pcb_t * OS_pcb;

scheduler_queue * create_queue_array(int quantum);
int remove_from_queue_by_pid(queue_t * queue, int pid);
pcb_t * get_idle_pcb();
void kill_processes_in_removal_queue();
void create_processes_in_creation_queue();
//int remove_process_from_removal_queue();

pcb_t * find_next_process( pcb_t * pcb ) {
    pcb_t * keep_running = NULL;
    for (int i = HIGH_PRIORITY; i >=LOW_PRIORITY; i--) {
        node_t * node = scheduler[i]->queue->current_node;
        while (node) {
            pcb_t *pcb = (pcb_t *)node->data;
            if (pcb->process->status == ZOMBIE) {
                node = node->next;
                continue;
            }
            if (pcb->process->status == RUNNING) { // there is only one process that should be with status RUNNING: the current process
                keep_running = pcb;
            }
            if (pcb->process->status == READY) {
                pcb->process->status = RUNNING;
                // scheduler[i]->queue->current_node = node->next;
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
    /*if(pid == 0){
        return OS_pcb;
    }*/
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
    //init_OS_pcb();
    for(int i=0 ; i<QUEUE_QTY ; i++){
        scheduler[i]=create_queue_array(quantum);
        if(scheduler[i] == NULL){
            return;
        }
    }
    waiting_for_remove = create_queue();
    if(waiting_for_remove == NULL){
        // maybe we should do smth here
        return;
    }
    creation_queue = create_queue();
    if(creation_queue == NULL){
        // maybe we should do smth here
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

/*
int remove_process_from_removal_queue(){
    node_t * node = waiting_for_remove->current_node;
    if(node == NULL || waiting_for_remove->qty == 0){
        return -1;
    }
    int pid = (int)(uintptr_t)node->data;
    remove_node(waiting_for_remove, node);
    return pid;
}
*/

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



void print_process() {
    // Constants defining the width of each column
    const int NAME_WIDTH = 16;
    const int STATUS_WIDTH = 12;
    const int PID_WIDTH = 8;
    const int PRIORITY_WIDTH = 10;

    // Draw the headers with padding
    drawWordPadded(WHITE, "\nName", NAME_WIDTH);
    drawWordPadded(WHITE, "Status", STATUS_WIDTH);
    drawWordPadded(WHITE, "PID", PID_WIDTH);
    drawWordPadded(WHITE, "Priority", PRIORITY_WIDTH);
    drawWordPadded(WHITE, "Parent PID", PID_WIDTH);
    drawWord("\n");

    for (int i = HIGH_PRIORITY; i >= 0; i--) {
        node_t *node = scheduler[i]->queue->current_node;

        while (node) {
            pcb_t *pcb = (pcb_t *)node->data;

            drawWordPadded(WHITE, pcb->process->name, NAME_WIDTH);
            drawWordPadded(WHITE, status_arr[(int)pcb->process->status], STATUS_WIDTH);
            drawNumberPadded(WHITE, pcb->process->pid, PID_WIDTH);
            drawNumberPadded(WHITE, i, PRIORITY_WIDTH); 
            drawNumberPadded(WHITE, pcb->process->parent_pid, PID_WIDTH);
            drawWord("\n");

            // Move to the next node
            node = node->next;

            if (node == scheduler[i]->queue->current_node) {
                break;
            }
        }
    }
}












uintptr_t * switch_context(uintptr_t * current_rsp) {
    current_pcb->process->stack->current = current_rsp; // save current rsp for next time
    //drawWord("\n");
    //drawWordColor(RED, current_pcb->process->name);
    kill_processes_in_removal_queue();
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


int remove_from_queue_by_pid(queue_t * queue, int pid) {
    node_t * current_node = queue->current_node;
    while (current_node) {
        pcb_t *pcb = (pcb_t *)current_node->data;

        if (pcb->process->pid == pid) {
            pcb->priority=ZOMBIE;
            remove_node(queue, current_node);
            return ZOMBIE;
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
    
    pcb->priority =  remove_from_queue_by_pid(scheduler[pcb->priority]->queue, pid);
    
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
    // Im debuggin. print all elements from the scheduler array
/*    for(int i=0 ; i<QUEUE_QTY ; i++){
        node_t * current_node = scheduler[i]->queue->current_node;
        drawWord("\n");
        drawWordColor(RED, "QUEUE: ");
        drawNumber(i);
        drawWordColor(RED, "  ");
        while (current_node) {
            pcb_t *pcb = (pcb_t *)current_node->data;
            drawWordColor(RED, pcb->process->name);
            drawWordColor(RED, "  ");
            current_node = current_node->next;
            if (current_node == scheduler[i]->queue->current_node) {
                break; 
            }
        }
    }
    drawWord("\n");
    drawWordColor(RED, "CURRENT PROCESS: ");
    drawNumber(current_pcb->process->pid);
*/

    if (current_pcb->ticks >= scheduler[current_pcb->priority]->quantum) {        
        
        if (current_pcb->priority > LOW_PRIORITY && current_pcb->priority <= HIGH_PRIORITY) {
           change_process_priority(current_pcb->process->pid, current_pcb->priority - 1);
        } else if(current_pcb->priority == LOW_PRIORITY || current_pcb->priority==IDLE_PRIORITY) {
            scheduler[LOW_PRIORITY]->queue->current_node = scheduler[LOW_PRIORITY]->queue->current_node->next;
        }
        
    } else {
        // in this csae the process is blocked or finished. We just advance the current node
        while(1){
            drawWord("-ERROR-");
        }
        scheduler[current_pcb->priority]->queue->current_node = scheduler[current_pcb->priority]->queue->current_node->next;
    }
}

int getQuantum(){
    return scheduler[current_pcb->priority]->quantum;
}

void reassign_children_to_shell(int old_parent_pid) {
    for (int i = 0; i < QUEUE_QTY; i++) {
        node_t *current_node = scheduler[i]->queue->current_node;
        if (current_node == NULL) {
            continue;
        }
        do {
            pcb_t *current_pcb = (pcb_t *)current_node->data;
            if (current_pcb->process->parent_pid == old_parent_pid) {
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

void os_revive_process(int pid) {
    pcb_t *pcb = get_pcb_entry(pid); // Fetch the PCB for the given PID
    if (pcb != NULL && (pcb->process->status == BLOCKED || pcb->process->status == RUNNING)) {
        pcb->process->status = READY; // Change status to READY
        // Add the process back to its respective priority queue
/*        node_t * pcb_node = create_node(pcb);
        if (pcb_node != NULL) {
            add_pcb_to_scheduler(pcb_node, pcb->priority);
        }*/
    }
}


int os_block_current_process() {
    if (current_pcb == NULL) {
        return -1; // Error if there is no current process
    }

    current_pcb->process->status = BLOCKED; // Change status to BLOCKED
    // Remove the process from its queue
    //remove_from_queue_by_pid(scheduler[current_pcb->priority]->queue, current_pcb->process->pid);
    // Trigger a context switch
    //force_context_switch((uintptr_t *)current_pcb->process->stack->current);
    return current_pcb->process->pid; // Return the PID of the blocked process
}

int block_process(int pid) {
    pcb_t *pcb = get_pcb_entry(pid); // Fetch the PCB for the given PID
    if (pcb == NULL) {
        return -1; // Process not found
    }
    
    // Toggle the status of the process based on its current state
    if (pcb->process->status == READY) {
        pcb->process->status = BLOCKED; // Change status to BLOCKED
        // If the process to block is the current running process, then force a context switch
        if (pcb == current_pcb) {
            force_context_switch((uintptr_t *)pcb->process->stack->current);
        }
        return 1; // Indicate the process was successfully blocked
    } else if (pcb->process->status == BLOCKED) {
        pcb->process->status = READY; // Change status to READY
        // No need to add it back to the queue, as we are not removing it when blocking
        return 2; // Indicate the process was successfully unblocked
    }
    
    return -1; // Process is in a state where it cannot be blocked/unblocked
}



int add_process_to_creation_queue(int parent_pid, char * name, size_t heap_size, size_t stack_size, void * entry_point, void * args){
    process_t * new_process = create_process(parent_pid, name, heap_size, stack_size, entry_point, args);
    if (new_process == NULL) {
        return -1; // Error if the process could not be created
    }
    node_t * new_node = create_node(new_process);
    if (new_node == NULL) {
        return -1; // Error if the node could not be created
    }
    if (insert_node(creation_queue, new_node) < 0) {
        return -1; // Error if the node could not be inserted
    }
    //drawWord("\n");
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
