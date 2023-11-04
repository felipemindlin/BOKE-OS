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
    }
}


uint64_t my_sem_open(uint64_t start_value, char *id) {
    int sem_idx = locate_sem(id);
    if (sem_idx != -1) {
        return -1;  // Semaphore with the same name already exists
    }
    
    for (int i = 0; i < MAX_SEMS; i++) {
        if (sem_spaces[i].status == AVAILABLE) {
            sem_spaces[i].status = UNAVAILABLE;
            mySem_t *sem = &(sem_spaces[i].sem);
            sem->counter = start_value;
            sem->is_locked = (start_value == 0) ? 1 : 0;
            str_cpy(sem->identifier, id);
            sem->users_count = 1;  // Creator is the first user
            sem->queue_size = 0;
            sem->head = NULL;
            sem->tail = NULL;
            return i;
        }
    }
    return -1;
}

void my_sem_close(int sem_idx) {
    if (sem_idx < 0 || sem_idx >= MAX_SEMS) {
        return;
    }
    while (sem_spaces[sem_idx].sem.queue_size > 0) {
        remove_from_queue(sem_idx);
    }
    sem_spaces[sem_idx].status = AVAILABLE;
}

uint64_t my_sem_post(uint64_t sem_idx) {
    if (sem_idx >= MAX_SEMS || sem_idx < 0) {
        return 1;
    }

    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    uint64_t *lock_addr = &(sem->is_locked);

//    enter_region(lock_addr, sem_idx);

    sem->counter++;

/*    if (sem->queue_size > 0) {
        int pid = remove_from_queue(sem_idx);
        os_revive_process(pid);
    }
*/
    leave_region(lock_addr, sem_idx);
    return 0;
}

uint64_t my_sem_wait(uint64_t sem_idx) {
    if (sem_idx >= MAX_SEMS || sem_idx < 0) {
        return 1;
    }

    mySem_t *sem = &(sem_spaces[sem_idx].sem);
    uint64_t *lock_addr = &(sem->is_locked);

    enter_region(lock_addr, sem_idx);

    if (sem->counter > 0) {
        sem->counter--;
//        leave_region(lock_addr, sem_idx);
//        return 0;
    }

//    int pid = current_process_id();
//    add_to_queue(sem_idx, pid);
//    os_block_current_process();
//    leave_region(lock_addr, sem_idx);

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
    sem->queue_size--;

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

int get_sem(char *id, int start_value) {
    int sem_idx = locate_sem(id);
    if (sem_idx == -1) {
        sem_idx = my_sem_open(start_value, id);
    }
    return sem_idx;
}

int put_sem(char *id) {
    int sem_idx = locate_sem(id);
    if (sem_idx == -1) {
        return -1;
    }
    my_sem_close(sem_idx);
    return 0;
}

void whiff(uint64_t sem_idx) {
  
    int pid = current_process_id();
    if (pid < 0) {
        return;
    }

    add_to_queue(sem_idx, pid);
    os_block_current_process();
}

void wake_up_processes(int sem_idx) {
    while (sem_spaces[sem_idx].sem.queue_size > 0) {
        int pid = remove_from_queue(sem_idx);
        os_revive_process(pid);
    }
}
