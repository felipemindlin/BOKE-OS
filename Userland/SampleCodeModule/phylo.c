#include <UserSyscalls.h>
#include <utils.h>

#define MAX_PHILOSOPHERS 20
#define MIN_PHILOSOPHERS 2
#define START_PHILOSOPHERS 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define SIZE 0x0000000000010000
#define R_W_SEMNAME "r_w_sem"

int num_philosophers = 0;
volatile int last = 0;
uint64_t mutex_id;
int fork_states[MAX_PHILOSOPHERS];
uint64_t fork_sem_ids[MAX_PHILOSOPHERS];
int philosopher_pids[MAX_PHILOSOPHERS];
int r_w_sem_index;
int add_remove_mutex_index;

void philosopher(uint64_t id);
void take_forks(uint64_t philosopher_num);
void put_forks(uint64_t philosopher_num);
void test(uint64_t philosopher_num);
void eat(uint64_t philosopher_num);
void think(uint64_t philosopher_num);
void add_philosopher();
void remove_philosopher();
void print_state();
void handle();

int left(int i) {
    return (i + num_philosophers - 1) % num_philosophers;
}

int right(int i) {
    return (i + 1) % num_philosophers;
}

void intialize(){
    num_philosophers = 0;
    last = 0;

}

int phylo() {
    intialize(); 
    mutex_id = call_sem_open(1, "mutex");
    r_w_sem_index = call_sem_open(1, R_W_SEMNAME);
    add_remove_mutex_index = call_sem_open(1, "philo_ar_mutex");

    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        char sem_name[10];
        itoa(i, sem_name, 10);
        fork_sem_ids[i] = call_sem_open(0, sem_name); // Semaphore initialized to 0
        fork_states[i] = THINKING;
    }

    print("Welcome to the dining philosophers problem.\nPress 'A' to add a philosopher, 'R' to remove one, or 'Q' to quit.\n");

    for (int i = 0; i < START_PHILOSOPHERS; i++) {
        add_philosopher();
    }
    handle();
    // Clean up philosophers
    for (int i = 0; i < num_philosophers; i++) {
        call_force_kill(philosopher_pids[i]);
        call_sem_close(fork_sem_ids[i]);
    }

    call_sem_close(mutex_id);
    call_sem_close(r_w_sem_index);
    call_sem_close(add_remove_mutex_index);

    return 0;
}

void handle() {
    char c;
    while (!last) {
        c = getC();
        if (c == 'A' || c == 'a') {
            if (num_philosophers < MAX_PHILOSOPHERS) {
                add_philosopher();
            }
        } else if (c == 'R' || c == 'r') {
            if (num_philosophers > MIN_PHILOSOPHERS) {
                remove_philosopher();
            }
        } else if (c == 'Q' || c == 'q') {
            last = 1;
        }
    }
}

void add_philosopher() {
    call_sem_wait(mutex_id);
    int philosopher_num = num_philosophers;
    fork_states[philosopher_num] = THINKING;
    char philosopher_id[12];
    itoa(philosopher_num, philosopher_id, 10);
    philosopher_pids[philosopher_num] = call_create_process(philosopher_id, SIZE, SIZE, philosopher, (void *)(uint64_t)philosopher_num);
    if (philosopher_pids[philosopher_num] == -1) {
        print("Failed to create philosopher process.\n");
        call_sem_post(mutex_id);
        return;
    }
    num_philosophers++;
    test(philosopher_num);
    call_sem_post(mutex_id);
}

void remove_philosopher() {
    call_sem_wait(add_remove_mutex_index);
    if (num_philosophers > MIN_PHILOSOPHERS) {
        int philosopher_num = num_philosophers - 1;
        if (fork_states[philosopher_num] == EATING) {
            call_sem_post(add_remove_mutex_index);
            call_sleepms(100);
            call_sem_wait(add_remove_mutex_index);
        }
        int left = (philosopher_num + num_philosophers - 1) % num_philosophers;
        int right = (philosopher_num + 1) % num_philosophers;
        test(left);
        test(right);
        call_force_kill(philosopher_pids[philosopher_num]);
        fork_states[philosopher_num] = THINKING;
        call_sem_close(fork_sem_ids[philosopher_num]);
        num_philosophers--;
    }
    call_sem_post(add_remove_mutex_index);
}

void philosopher(uint64_t id) {
    while (!last) {
        think(id);
        take_forks(id);
        eat(id);
        put_forks(id);
    }
}

void take_forks(uint64_t philosopher_num) {
    call_sem_wait(mutex_id);
    fork_states[philosopher_num] = HUNGRY;
    call_sem_post(r_w_sem_index);
    test(philosopher_num);
    if (fork_states[philosopher_num] != EATING) {
        call_sem_post(mutex_id);
        call_sem_wait(fork_sem_ids[philosopher_num]);
    } else {
        call_sem_post(mutex_id);
    }
}

void put_forks(uint64_t philosopher_num) {
    call_sem_wait(mutex_id);
    fork_states[philosopher_num] = THINKING;
    call_sem_post(r_w_sem_index);
    test(left(philosopher_num));
    test(right(philosopher_num));
    call_sem_post(mutex_id);
}

void test(uint64_t philosopher_num) {
    if (fork_states[philosopher_num] == HUNGRY &&
        fork_states[left(philosopher_num)] != EATING &&
        fork_states[right(philosopher_num)] != EATING) {
        fork_states[philosopher_num] = EATING;
        call_sem_post(fork_sem_ids[philosopher_num]);
    }
}

void think(uint64_t philosopher_num) {
    call_sleepms(100);
}

void eat(uint64_t philosopher_num) {
    if (fork_states[philosopher_num] == EATING) {
        print_state();
        call_sleepms(100);
    }
}

void print_state() {
    call_sem_wait(mutex_id);
    print("State of the table with %d philosophers: ", num_philosophers);
    for (int i = 0; i < num_philosophers; i++) {
        if (fork_states[i] == EATING) {
            print("E ");
        } else {
            print(". ");
        }
    }
    print("\n");
    call_sem_post(mutex_id);
}
