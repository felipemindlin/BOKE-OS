#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H
#include <stdint.h>
#include <UserSyscalls.h>
#include <utils.h>

// HARD CAP AT 0x1000000 ALLOC
#define TEST_MAXMEM 0x100000

void tests();
uint64_t test_mm();
void test_prio();
int64_t test_processes();
uint64_t test_sync(char **argv);

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop_print();

#endif