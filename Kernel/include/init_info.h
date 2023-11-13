#ifndef INIT_INFO_H
#define INIT_INFO_H

#include <stdint.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernelBinary;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;

#endif