#include "include/memory_manager.h"

#define MAX 1001
#define PAGE_SIZE 32
#define MAX_MEM 408944640

heap heap_struct; //TO-DO: Implement ADT so we just reserve the memory needed for one memory manager

static uintptr_t get_page_index(void * mem);
static uintptr_t get_first_page_index(void *¨mem);
static void initialize_pages();
static uintptr_t find_pages(size_t pages_needed, uintptr_t start_page);
static size_t size_to_num_of_pages(size_t size);

void init_mm(void * baseAddres, uint64_t mem_ammount){

    size_t total_mem_needed=mem_ammount;
    size_t bitmap_size = mem_ammount / PAGE_SIZE;
    if (mem_ammount % PAGE_SIZE != 0){
        bitmap_size++;
        total_mem_needed = bitmap_size * PAGE_SIZE;
    }

    size_t bitmap_pages = bitmap_size/PAGE_SIZE;
    if(bitmap_size % PAGE_SIZE != 0){
        bitmap_pages++;
    }
    total_mem_needed+=bitmap_pages*PAGE_SIZE;

    if(total_mem_needed>MAX_MEM){
        drawWord("\nNot enough memory to initialize heap!\n");
        return;
    }

    if(bitmap_pages > bitmap_size){
        drawWord("\nNot enough memory to initialize heap!\n");
        return;
    }

    heap_struct.page_size = PAGE_SIZE;
    heap_struct.page_qty = bitmap_size;
    heap_struct.start = (uintptr_t) baseAddres + (uintptr_t) bitmap_pages * PAGE_SIZE;
    heap_struct.size = total_mem_needed;
    heap_struct.end = (uintptr_t) baseAddres + total_mem_needed;
    heap_struct.last_freed_page = 0;
    heap_struct.pages = (uint8_t*) baseAddres;

    initialize_pages();
}

static void initialize_pages(){
    for (uintptr_t i = 0; i < heap_struct.page_qty; i++){
        heap_struct.pages[i] = FREE;
    }
}

static size_t size_to_num_of_pages(size_t size){
    size_t pages_needed = size / heap_struct.page_size;
    if (size % heap_struct.page_size != 0){
        pages_needed++;
    }
    return pages_needed;
}

static uintptr_t find_pages(size_t pages_needed, uintptr_t start_page){
    uintptr_t page_index = 0;
    uintptr_t pages_found = 0;
    for (uintptr_t i = start_page; i < heap_struct.page_qty; i++){
        if (heap_struct.pages[i] == FREE){
            pages_found++;
            if (pages_found == pages_needed){
                page_index = i - pages_needed + 1;
                return page_index;
            }
        }
        else{
            pages_found = 0;
        }
    }
    return INVALID_ADDRESS;
}

void* malloc(size_t size){
    size_t pages_needed = size_to_num_of_pages(size);
    uintptr_t page_index = find_pages(pages_needed, heap_struct.last_freed_page);
    if(page_index==INVALID_ADDRESS){
        page_index = find_pages(pages_needed, 0);
    }
    if(page_index==INVALID_ADDRESS){
        drawWord("\nRan out of memory!\n");
        return NULL;
    }

    for (uintptr_t i = page_index; i < page_index + pages_needed; i++){
        if(i==page_index){ 
            if(pages_needed==1){
                heap_struct.pages[i] = SINGLE_PAGE;
            } else{
                heap_struct.pages[i] = START_PAGE_BOUND;
            }
        } else if(i==page_index+pages_needed-1){ // last page
            heap_struct.pages[i] = END_PAGE_BOUND;
        } else{
            heap_struct.pages[i] = ALLOCATED;
        }
    }

    return (void*)( heap_struct.start + page_index * heap_struct.page_size);
}


// the free recieves a void *. It must free the complete block of memory that was allocated. From page_boud to page_bound
void free(void * mem){
    uintptr_t page_index = get_page_index(mem);
    if(page_index==INVALID_ADDRESS){
        drawWord("\nInvalid address\n");
        return;
    }
    if(heap_struct.pages[page_index]==FREE){
        drawWord("\nInvalid address\n");
        return;
    }
    if(heap_struct.pages[page_index]==SINGLE_PAGE){
        heap_struct.pages[page_index]=FREE;
        return;
    }
    uintptr_t i=get_first_page_index(mem);
    while(heap_struct.pages[i]!=END_PAGE_BOUND){
        heap_struct.pages[i]=FREE;
        i++;
    }
    heap_struct.pages[i]=FREE;
}


static uintptr_t get_page_index(void * mem){
    if((uintptr_t)mem > heap_struct.end || (uintptr_t) mem < heap_struct.start){
        drawWord("\nInvalid address!\n");
        return INVALID_ADDRESS;
    }
    return ((uintptr_t)mem - heap_struct.start) / heap_struct.page_size;
}

static uintptr_t get_first_page_index(void * mem){
    if((uintptr_t)mem > heap_struct.end || (uintptr_t) mem < heap_struct.start){
        drawWord("\nInvalid address!\n");
        return INVALID_ADDRESS;
    }
    uintptr_t page_idx = get_page_index(mem);
    if(heap_struct.pages[page_idx]==SINGLE_PAGE){
        return page_idx;
    }
    if(heap_struct.pages[page_idx]==START_PAGE_BOUND){
        return page_idx;
    }
    if(heap_struct.pages[page_idx]==END_PAGE_BOUND || heap_struct.pages[page_idx]==ALLOCATED){
        uintptr_t i=page_idx;
        while(heap_struct.pages[i]!=START_PAGE_BOUND){
            i--;
        }
        return i;
    }
    return INVALID_ADDRESS;
}

int calculate_free_pages(){
    int free_pages = 0;
    for (uintptr_t i = 0; i < heap_struct.page_qty; i++){
        if (heap_struct.pages[i] == FREE)
            free_pages++;
    }
    return free_pages;
}

void printMem(){
    int free_pages = calculate_free_pages();
    drawWord("\nTotal memory in bytes: ");
    drawNumber(heap_struct.size);
    drawWord("\nUsed memory in bytes: ");
    drawNumber(heap_struct.size - free_pages * heap_struct.page_size);
    drawWord("\nFree memory in bytes: ");
    drawNumber(free_pages * heap_struct.page_size);
}
