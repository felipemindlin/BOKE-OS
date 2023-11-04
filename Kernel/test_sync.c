#include <stdint.h>
#include <string.h>
#include "drivers/include/videoDriver.h"
#define SEM_ID "sem_sem"
#define TOTAL_PAIR_PROCESSES 4
#include <libasm.h>
#include <process.h>
#include <mysemaphore.h>

void my_yield(){
  forceTimer();
}
int64_t satoi(char *str) {
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str)
    return 0;

  if (str[i] == '-') {
    i++;
    sign = -1;
  }

  for (; str[i] != '\0'; ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}
int64_t global; // shared memory
void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  my_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}
int flag=0;
 void my_process_inc(char *argv[]) {
  /*drawWord("Im in processINC\n");
  drawWord("argc=");
  drawNumber(argc);
  drawWord("\n");
  drawWord("argv[0]=");
  drawWord(argv[0]);
  drawWord("\n");
  drawWord("argv[1]=");
  drawWord(argv[1]);
  drawWord("\n");
  drawWord("argv[2]=");
  drawWord(argv[2]);
  drawWord("\n");
  drawWord("argv[3]=");
  drawWord(argv[3]);
*/
// void my_process_inc() {
  int64_t n = satoi(argv[1]);
  int8_t inc = satoi(argv[2]);
  int8_t use_sem = satoi(argv[3]);

  int my_sem = -1;

  if (n <= 0 || n > 10)
    n = 4;

  if (use_sem) {
    if ((my_sem = my_sem_open(SEM_ID, 1)) == -1) {
      drawWord("test_sync: ERROR opening semaphore\n");
      return;
    }
  }

  uint64_t i;
  for (i = 0; i < n; i++) {
    //drawNumber(i);
    //drawWord("\n");
    if (use_sem) {
      my_sem_wait(my_sem);
    }
    slowInc(&global, inc);
    if (use_sem) {
      my_sem_post(my_sem);
    }
    //drawNumber(i);
    //drawWord("\n");
  }

  return;
}
uint64_t test_sync(char *argv[]) { //{n, use_sem}
/*  drawWord("Im in test-sync\n");
  drawWord("\n");
  drawWord("argv[0]=");
  drawWord(argv[0]);
  drawWord("\n");
  drawWord("argv[1]=");
  drawWord(argv[1]);
  drawWord("\n");*/
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  char *argvDec[] = {"Decreaser", argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {"Increaser", argv[0], "1", argv[1], NULL};

  global = 0;

  // Assuming you have a defined process size and parent_pid for this context
  size_t process_heap_size = 0x2000; // Example heap size
  size_t process_stack_size = 0x2000; // Example stack size
  int parent_pid = 0; // Assuming '0' is the PID for the initial process or kernel

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    // Here we need to pass all the required parameters to create_and_insert_process
    //drawNumber(i);
    pids[i] = create_and_insert_process(parent_pid, "Decreaser", process_heap_size, process_stack_size, (void *)my_process_inc, argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = create_and_insert_process(parent_pid, "Increaser", process_heap_size, process_stack_size, (void *)my_process_inc, argvInc);
    // pids[i] = create_and_insert_process(parent_pid, "Decreaser", process_heap_size, process_stack_size, (void *)my_process_inc, NULL);
    // pids[i + TOTAL_PAIR_PROCESSES] = create_and_insert_process(parent_pid, "Increaser", process_heap_size, process_stack_size, (void *)my_process_inc, NULL);
 
  }

  // Assuming you have a my_sem_wait function to wait for the process to complete
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    my_sem_wait(pids[i]);
    my_sem_wait(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  /*drawWord("\nExpected value: 0\n");
  drawWord("Final value:");
  drawNumber((int)global);
  drawWord("\n");*/

  return 0;
}