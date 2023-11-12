#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#include <stdint.h>
#include <init_info.h>
#include "../drivers/include/videoDriver.h"


enum { INVALID_ADDRESS=-1, FREE, ALLOCATED, PARCIAL, START_PAGE_BOUND, END_PAGE_BOUND, SINGLE_PAGE };

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
void free(void * mem);
void printMem();

#endif