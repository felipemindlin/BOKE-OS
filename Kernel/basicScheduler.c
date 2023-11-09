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
            if (pcb->priority == DEPRECATED) {
                node = node->next;
                continue;
            }
            if (pcb->process->status == RUNNING) { // there is only one process that should be with status RUNNING: the current process
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

void print_process(){
    drawWord("\n");
  for (int i = HIGH_PRIORITY; i >=0; i--) {
        node_t * node = scheduler[i]->queue->current_node;
        drawWord("PROCESSES IN PRIORITY: ");
        drawNumber(i);
        drawWord("\n");
        while (node) {
             node = node->next;
            drawWord("Name: ");
            drawWord(((pcb_t *)node->data)->process->name);
            drawWord("\n");

            drawWord("Status: ");
            drawWord(status_arr[(int)((pcb_t *)node->data)->process->status]);
            drawWord("\n");

            drawWord("PID: ");
            drawNumber(((pcb_t *)node->data)->process->pid);
            drawWord("\n");
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
    //drawWord("PID removed from queue: ");
    //drawNumber(pid);
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

void kill_processes_in_removal_queue(){
    int pid = (int) (uintptr_t) remove_node(waiting_for_remove, waiting_for_remove->current_node);
    while(pid){
        remove_from_queue_by_pid(scheduler[get_pcb_entry(pid)->priority]->queue, pid);
        free_process(get_pcb_entry(pid));
        pid = (int) (uintptr_t) remove_node(waiting_for_remove, waiting_for_remove->current_node);
    }
}


/*
void init_OS_pcb(){
    OS_pcb = (pcb_t *) malloc(sizeof(pcb_t));
    if(OS_pcb == NULL){
        return;
    }
    OS_pcb->priority = ZOMBIE; // sus
    OS_pcb->ticks = 0;
    process_t * process = (process_t *) malloc(sizeof(process_t));
    if(process == NULL){
        free(OS_pcb);
        return;
    }
    OS_pcb->process->parent_pid = 0;
    process->pid = 0;
    process->status = RUNNING; // imagine it is always running, though it is clearly not always the current process
    process->name = malloc(str_len("OS") + 1);
    if(process->name == NULL){
        free(process);
        free(OS_pcb);
        return;
    }
    str_cpy(process->name, "OS");
    process->stack = (mem_block_t *) malloc(sizeof(mem_block_t));
    if(process->stack == NULL){
        free(OS_pcb);
        return;
    }
    process->stack->base = NULL;
    process->stack->size = 0;
    process->stack->current = NULL;
    process->heap = (mem_block_t *) malloc(sizeof(mem_block_t));
    if(process->heap == NULL){
        free(process->stack);
        free(OS_pcb);
        return;
    }
    process->heap->base = NULL;
    process->heap->size = 0;
    process->heap->current = NULL;
    OS_pcb->process = process;
    processes_qty++;
    // do not add OS to scheduler, it runs when it wants
}
*/


int current_process_id(){
    if(current_pcb == NULL){
        return -1;
    }
    return current_pcb->process->pid;
}

void os_revive_process(int pid) {
    pcb_t *pcb = get_pcb_entry(pid); // Fetch the PCB for the given PID
    if (pcb != NULL && pcb->process->status == BLOCKED) {
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

void block_process(int pid) {
    pcb_t *pcb = get_pcb_entry(pid); // Fetch the PCB for the given PID
    if (pcb != NULL && pcb->process->status == READY) {
        pcb->process->status = BLOCKED; // Change status to BLOCKED
        remove_from_queue_by_pid(scheduler[pcb->priority]->queue, pcb->process->pid);
        force_context_switch((uintptr_t *)pcb->process->stack->current);
    } else if (pcb != NULL && pcb->process->status == BLOCKED) {
        pcb->process->status = READY; // Change status to READY
        add_pcb_to_scheduler(scheduler[pcb->priority]->queue->current_node, pcb->process->pid);
    }
}

int add_process_to_creation_queue(int parent_pid, char * name, size_t stack_size, size_t heap_size, void * entry_point, void * args,int fd[2]){
    process_t * new_process = create_process(parent_pid, name, heap_size, stack_size, entry_point, args,fd[0],fd[1]);
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