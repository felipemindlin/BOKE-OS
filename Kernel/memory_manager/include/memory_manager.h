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
#define FREE 0
#define ALLOCATED 1
#define PARCIAL 2

#define	__need_size_t
#define	__need_NULL
#include <stddef.h>

typedef struct heap {
    uintptr_t start;
    uint64_t size;
    uintptr_t end;
    uintptr_t page_qty;
    uintptr_t page_size;
    uintptr_t last_freed_page;
    uint8_t * pages;
} heap;

typedef struct Node {
    void * start;
    uint64_t size;
    uint64_t state;
    uint64_t index;
    struct Node *left;  
    struct Node *right; 
} Node;

void init_mm(void * baseAddres, uint64_t mem_ammount);
void initialize_heap(void * baseAddres, uint64_t mem_ammount);
void * malloc(uintptr_t bytes);
void * free(void * mem);
void printMem();

#endif