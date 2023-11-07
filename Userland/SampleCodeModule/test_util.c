#include <test_util.h>
#include <utils.h>


void tests() {
    print("Usage: TEST <test_number> <param_1>\n");
    print("Available tests:\n");
    print("  A - Test memory manager <max_mem>\n");
    print("  B - Test priority\n");
    print("  C - Test processes <max_processes>\n");
    print("  D - Test synchronization <n>\n");
    print("  E - Test without synchronization <n>\n");
    print("  Q - Quit\n");
    print("Press enter to confrim selection");
    char c;
    int test_num = 0;
    while ((c = getC()) != 'Q' && test_num == 0) {
        print("%c", c); // Use %c to print a character.
        if (c >= 'A' && c <= 'E') {
            test_num = c - 'A' + 1;
        } else {
            print("Invalid test number\n Try again\n");
        }
    }

    if (c == 'Q') {
        print("Exiting...\n");
        return;
    }

    char *argv_aux[2] = {"3", "1"};

    switch (test_num) {
        case 1:
            test_mm();
            break;
        case 2:
            test_prio();
            break;
        case 3:
            test_processes();
            break;
        case 4:
            test_sync(argv_aux);
            break;
        case 5:
            argv_aux[1] = "0";
            test_sync(argv_aux);
            break;
    }
}
// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint() {
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max) {
  uint32_t u = GetUint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t memcheck(void *start, uint8_t value, uint32_t size) {
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
      return 0;

  return 1;
}

// Dummies
void bussy_wait(uint64_t n) {
  uint64_t i;
  for (i = 0; i < n; i++)
    ;
}

void endless_loop() {
  while (1)
    ;
}

void endless_loop_print() {
  int pid = get_pid();

  int wait = 10000;

  while (1) {
    print("%d ", pid);
    bussy_wait(wait);
  }
}