#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include "time.h"

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

int str_cpy(char * dst, const char * src);
int strcmp(const char * str1, const char * str2);
int str_len(const char * str);

void sleep(uint32_t seconds);
void sleepms(int mseconds);
void nanoms(int nseconds);
char * cpu_vendor(char* vendor);

void force_context_switch(uintptr_t * rsp);
void * create_stackframe(void * entry_point, char ** args, void * stack_base, void * wrapper);

void guru_meditation();

void force_scheduler();

#endif /* LIB_H */