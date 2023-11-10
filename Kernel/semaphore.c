#include <mysemaphore.h>
#include <string.h>
#include <scheduler.h>
#include "../memory_manager/include/memory_manager.h"

extern void enter_region(uint64_t *lock, uint64_t sem_idx);
extern void leave_region(uint64_t *lock, uint64_t sem_idx);

extern void os_revive_process(int pid);
extern int os_block_current_process();

typedef struct {
    mySem_t sem;
    uint64_t status;
} space_t;

static space_t sem_spaces[MAX_SEMS];

void initialize_sems() {
    for (int i = 0; i < MAX_SEMS; i++) {
        sem_spaces[i].status = AVAILABLE;
        for (int j = 0; j < MAX_PROCESSES_PER_SEM; j++) {
            sem_spaces[i].sem.allowed_processes[j] = -1;
        }
        sem_spaces[i].sem.allowed_process_count = 0;
    }
}

uint64_t my_sem_open(uint64_t start_value, char *id) {
    int sem_idx = locate_sem(id);
    if (sem_idx != -1) {
        int creating_pid = current_process_id();
        sem_spaces[sem_idx].sem.allowed_processes[sem_spaces[sem_idx].sem.allowed_process_count++] = creating_pid;
        return sem_idx;
    }
    
    for (int i = 0; i < MAX_SEMS; i++) {
        if (sem_spaces[i].status == AVAILABLE) {
            sem_spaces[i].status = UNAVAILABLE;
            mySem_t *sem = &(sem_spaces[i].sem);
            sem->counter = start_value;
            sem->is_locked = (start_value > 0) ? 1 : 0;
            str_cpy(sem->identifier, id);
            sem->users_count = 1;  // Creator is the first user
            sem->queue_size = 0;
            sem->head = NULL;
            sem->tail = NULL;
            sem->allowed_process_count = 1;
            sem->allowed_processes[0] = current_process_id();
            sem->being_cleared = 0;
            return i;
        }
    }
    return -1;
}

void my_sem_close(char *id) {
    int sem_idx = locate_sem(id);
    if (sem_idx == -1) {
        return;  // Semaphore with this name does not exist
    }
    
    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    int current_pid = current_process_id();
    for (int i = 0; i < sem->allowed_process_count; i++) {
        if (sem->allowed_processes[i] == current_pid) {
            sem->allowed_processes[i] = -1;
            sem->users_count--;
        }
    }

    if (sem->users_count == 0) {
        clear_sem(id);
    }
}

uint64_t my_sem_post(char *id) {
    int sem_idx = locate_sem(id);
    if (sem_idx == -1) {
        return 1;  // Semaphore with this name does not exist
    }

    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    uint64_t *lock_addr = &(sem->is_locked);

    leave_region(lock_addr, sem_idx);
    return 0;
}

uint64_t my_sem_wait(char *id) {
    int sem_idx = locate_sem(id);
    if (sem_idx == -1) {
        return -1;  // Semaphore with this name does not exist
    }

    int current_pid = current_process_id();
    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    
    int allowed = 0;
     for (int i = 0; i < sem->allowed_process_count && !allowed; i++) {
         if (sem->allowed_processes[i] == current_pid) {
             allowed = 1;
         }
     }
    
     if (!allowed) {
         return -1;  // Current process is not allowed to use this semaphore
     }
    uint64_t *lock_addr = &(sem->is_locked);
    enter_region(lock_addr, sem_idx);
    return 0;
}


int add_to_queue(int sem_idx, int pid) {
    if (sem_idx < 0 || sem_idx >= MAX_SEMS) {
        return -1;
    }

    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    node_t *new_node = (node_t *)malloc(sizeof(node_t));

    if (!new_node) {
        return -1;
    }

    new_node->data = (void *)(uintptr_t)pid;
    new_node->next = NULL;
    new_node->prev = sem->tail;

    if (sem->tail) {
        sem->tail->next = new_node;
    } else {
        sem->head = new_node;
    }

    sem->tail = new_node;
    sem->queue_size++;

    return 0;
}

int remove_from_queue(int sem_idx) {
    if (sem_idx < 0 || sem_idx >= MAX_SEMS) {
        return -1;
    }

    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    if (!sem->head) {
        return -1;
    }

    node_t *node_to_remove = sem->head;
    int pid = (int)(uintptr_t)node_to_remove->data;

    sem->head = node_to_remove->next;
    if (sem->head) {
        sem->head->prev = NULL;
    } else {
        sem->tail = NULL;
    }

    free(node_to_remove);
    sem->queue_size-=1;
    if(sem->queue_size == 0){
        sem->head=NULL;
        sem->tail=NULL;
    }
    return pid;
}

int locate_sem(char *id) {
    for (int i = 0; i < MAX_SEMS; i++) {
        if (sem_spaces[i].status == UNAVAILABLE &&
            strcmp(sem_spaces[i].sem.identifier, id) == 0) {
            return i;
        }
    }
    return -1;
}

// int get_sem(char *id, int start_value) {
//     int sem_idx = locate_sem(id);
//     if (sem_idx == -1) {
//         sem_idx = my_sem_open(start_value, id);
//     }
//     return sem_idx;
// }

// int put_sem(char *id) {
//     int sem_idx = locate_sem(id);
//     if (sem_idx == -1) {
//         return -1;
//     }
//     my_sem_close(sem_idx);
//     return 0;
// }

void whiff(uint64_t sem_idx) {
  
    int pid = current_process_id();
    if (pid < 0) {
        return;
    }

    add_to_queue(sem_idx, pid);
    os_block_current_process();
    finish_current_tick();
}

void wake_up_processes(uint64_t sem_idx){
    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    uint64_t *lock_addr = &(sem->is_locked);

    for(int i = 0; i < sem->queue_size; i++){
        int pid = remove_from_queue(sem_idx);
        os_revive_process(pid);
    }

}

int clear_sem(char * sem_id){

    int sem_idx = locate_sem(sem_id);
    
    if(sem_idx == -1 || sem_spaces[sem_idx].sem.being_cleared){
        return -1;
    }

    sem_spaces[sem_idx].sem.being_cleared = 1;

    node_t * node = sem_spaces[sem_idx].sem.head;
    node_t * aux;

    while(node != NULL){
        aux = node->next;
        free(node);
        node = aux;
    }

    sem_spaces[sem_idx].sem.head = NULL;
    sem_spaces[sem_idx].sem.tail = NULL;
    sem_spaces[sem_idx].sem.queue_size = 0;

    for(int i = 0; i < sem_spaces[sem_idx].sem.allowed_process_count; i++){
        sem_spaces[sem_idx].sem.allowed_processes[i] = 0;
    }
    sem_spaces[sem_idx].sem.allowed_process_count = 0;
    sem_spaces[sem_idx].sem.users_count = 0;
    sem_spaces[sem_idx].sem.counter = 0;
    sem_spaces[sem_idx].sem.is_locked = 0;

    for(int i = 0; i < MAX_PROCESSES_PER_SEM; i++){
        sem_spaces[sem_idx].sem.identifier[i] = 0;
    }

    sem_spaces[sem_idx].status = AVAILABLE;

    return 0;
}
