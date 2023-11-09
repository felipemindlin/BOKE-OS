#include <stdint.h>
//#define SEM_ID "sem_sem"
#define TOTAL_PAIR_PROCESSES 4
#include <UserSyscalls.h>
#include <utils.h>
char * sem_id[]={"sem_1", "sem_2"};

char *argvDec[] = {"Decreaser", NULL, "-1", NULL, "sem_1"};
char *argvInc[] = {"Increaser", NULL, "1", NULL, "sem_1"};

void my_yield(){
  call_forceTimer();
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
int64_t global = 0; // shared memory
void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  my_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

 void my_process_inc(char *argv[]) {
  int64_t n = satoi(argv[1]);
  int8_t inc = satoi(argv[2]);
  int8_t use_sem = satoi(argv[3]);

  int my_sem = 1;


  if (n <= 0 || n > 10)
    n = 4;

  if (use_sem) {
    if ((my_sem = call_sem_open(1, argv[4])) == -1) {
      print("test_sync: ERROR opening semaphore\n");
      return;
    }
  }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem) {
      call_sem_wait(argv[4]);
    }
    slowInc(&global, inc);
    if (use_sem) {
      
      call_sem_post(argv[4]);
    }
  }
  print("Im finishing\n");
  return;
}
uint64_t test_sync(char *argv[]) { 

  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];


  argvDec[1]=argv[0];
  argvDec[3]=argv[1];

  argvInc[1]=argv[0];
  argvInc[3]=argv[1];

  global=0;

  size_t process_heap_size = 4096; // Example heap size
  size_t process_stack_size = 4096; // Example stack size

  size_t heap_and_stack[2] = {process_heap_size,process_stack_size};
  uint64_t i;
  int fd[2]={0,0};
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = call_create_process( "Decreaser",heap_and_stack, (void *)my_process_inc, argvDec,fd);
    pids[i + TOTAL_PAIR_PROCESSES] = call_create_process( "Increaser", heap_and_stack, (void *)my_process_inc, argvInc,fd);
  }
  int64_t idx = satoi(argv[1]);

 for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
   call_waitpid(pids[i]);
   call_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
 }
 print("\nExpected value: 0\n Final value:%d\n\n", (int)global);

  return 0;
}