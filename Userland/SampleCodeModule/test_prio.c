#include <test_util.h>
#include <stdint.h>
#include <UserSyscalls.h>


#define MINOR_WAIT 1000000  
#define WAIT  10000000       

#define TOTAL_PROCESSES 5
#define LOWEST 0
#define MEDIUM 3
#define HIGHEST 5

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST, MEDIUM, LOWEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  argv[0] = "startfull_loop_print";
  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = call_create_process("startfull_loop_print", 1, 0x0000000000010000, 0x0000000000010000, endless_loop_print, argv);

  bussy_wait(WAIT);
  print("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  print("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_block(pids[i]);

  print("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_nice(pids[i], MEDIUM);

  print("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_block(pids[i]);

  bussy_wait(WAIT);
  print("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    call_kill(pids[i]);

  print("\nTest ended. Processes were not printed because\n");
  print("they were created at background.\n");
}