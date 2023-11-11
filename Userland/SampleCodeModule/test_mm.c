#include <test_util.h>
#include <stdint.h>
#include <UserSyscalls.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;
void * memset(void * destiation, int32_t c, uint64_t length);

void * get_testmm(){
  return &test_mm;
}
void *user_malloc(uintptr_t bytes);
void user_free(void *mem);
uint64_t test_mm() {

  mm_rq mm_rqs[MAX_BLOCKS];
  // uint64_t max_memory = satoi(argv[0]);

  // if (max_memory == 0 || max_memory > TEST_MAXMEM)
    uint64_t max_memory = TEST_MAXMEM;

  print("Memory requested (capped at %d): %d\n", TEST_MAXMEM, (int)max_memory);
  while (1) {
    uint8_t rq = 0;
    uint32_t total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      //print("Size: %d - ", (int)mm_rqs[rq].size);
      mm_rqs[rq].address = user_malloc(mm_rqs[rq].size);
      //print("Addr: %x\n", (uint64_t)mm_rqs[rq].address);

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          print("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        //print("Free addr: %x\n", (uint64_t)mm_rqs[i].address);
        user_free(mm_rqs[i].address);
      }
  }
}