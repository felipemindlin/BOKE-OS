#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#include <stdint.h>
#include <init_info.h>
#include "../drivers/include/videoDriver.h"
/*
#define MAX 1001
#define START 0
#define END 1000
#define BLOCK_SIZE 10*/

/* Get size_t and NULL from <stddef.h>.  */
#define INVALID_ADDRESS -1
#define	__need_size_t
#define	__need_NULL
#include <stddef.h>

void initializeHeap();
void * malloc(uintptr_t bytes);
uintptr_t free(void * mem);

void printMem();

#endif