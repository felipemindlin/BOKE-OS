#include <test_util.h>
#include <stdint.h>
#include <UserSyscalls.h>


#define MINOR_WAIT 1000000  
#define WAIT  10000000       

#define TOTAL_PROCESSES 5
#define LOWEST 1
#define MEDIUM 2
#define HIGHEST 3

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST, MEDIUM, LOWEST};
void endless_loop_print() {
  int pid = get_pid();

  int wait = 100000000;

  while (1) {
    print("%d ", pid);
    bussy_wait(wait);
  }
}
void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[2] = {0};
  uint64_t i;

  argv[0] = "print_loop";
  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = call_create_process("print_loop", 0x0000000000010000, 0x0000000000010000, &endless_loop_print, NULL);

  bussy_wait(WAIT);
  print("\nCHANGING PRIORITIES...\n");
  for (i = 0; i < TOTAL_PROCESSES; i++)
    argv[0]=atoi(pids[i]);
    argv[1]=atoi(prio[i]);
    call_nice(argv);

  bussy_wait(WAIT);
  print("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_block(pids[i]);

  print("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    argv[0]=atoi(pids[i]);
    argv[1]=atoi(prio[i]);
    call_nice(argv);
  print("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_block(pids[i]);
  bussy_wait(WAIT);
  print("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_force_kill(pids[i]);

  print("\nTest ended. Processes were not printed because\n");
  print("they were created at background.\n");
}