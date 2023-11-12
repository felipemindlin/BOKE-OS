#ifndef _LIBASM_H
#define _LIBASM_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpu_vendor(char *result);
int get_hours();
int get_minutes();
int get_seconds();
int get_weekDay();
int get_monthDay();
int get_month();
int get_year();
void sleep(uint32_t seconds);
void sleepms(int mseconds);
int kbflag();
#endif /* LIB_H */