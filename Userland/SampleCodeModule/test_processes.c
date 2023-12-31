#include <test_util.h>
#include <stdint.h>
#include <UserSyscalls.h>
#define SIZE 0x0000000000010000
enum State { RUNNING, BLOCKED, KILLED };
int filede[2] = {0, 0};
size_t heap_stackd[2] = {0x0000000000001000, 0x0000000000001000};
typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;
void endless_loop(){
  while (1)
    ;
}



int64_t test_processes(){
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
   uint64_t max_processes;

    max_processes = 5;

  p_rq p_rqs[max_processes];

  print("Max processes per loop: %d\n", (int)max_processes);

  while (1){

        for (rq = 0; rq < max_processes; rq++){
          p_rqs[rq].pid = call_create_process("endless_loop",  0, heap_stackd, endless_loop, NULL, filede);

      print("Created PID: %d\n", (int)p_rqs[rq].pid);

      if (p_rqs[rq].pid == -1){
        print("test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

            while (alive > 0){

      for (rq = 0; rq < max_processes; rq++){
        action = get_uniform(100) % 2;

        switch (action){
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED){
            if (call_force_kill(p_rqs[rq].pid) == -1){
              print("test_processes: ERROR killing process\n");
              return -1;
            }
            print("Killed PID: %d\n", (int)p_rqs[rq].pid);
            p_rqs[rq].state = KILLED;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING){
            if (call_block(p_rqs[rq].pid) == -1){
              print("test_processes: ERROR blocking process\n");
              return -1;
            }
            print("Blocked PID: %d\n", (int)p_rqs[rq].pid);
            p_rqs[rq].state = BLOCKED;
          }
          break;
        }
      }

            for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && get_uniform(100) % 2){
          if (call_block(p_rqs[rq].pid) == -1){
            print("test_processes: ERROR unblocking process\n");
            return -1;
          }
          print("Unblocked PID: %d\n", (int)p_rqs[rq].pid);
          p_rqs[rq].state = RUNNING;
        }
    }
  }

  print("\nTest ended. Processes were not printed because\n");
  print("they were created at background.\n");
  return 0;
}
void * get_test_processes(){
  return &test_processes;
}