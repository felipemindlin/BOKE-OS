#include <memory_manager.h>

#define MAX 1001
#define PAGE_SIZE 16
#define MAX_MEM 200000

heap heap_struct; //TO-DO: Implement ADT so we just reserve the memory needed for one memory manager

static uintptr_t get_page_index(void * mem);
static void initialize_pages();
static uintptr_t find_pages(size_t pages_needed, uintptr_t start_page);
static size_t size_to_num_of_pages(size_t size);

void initialize_heap(void * baseAddres, uint64_t mem_ammount){

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
        drawWord("\nKERNEL PANIC: Not enough memory to initialize heap!\n");
        return;
    }

    if(bitmap_pages > bitmap_size){
        drawWord("\nKERNEL PANIC: Not enough memory to initialize heap!\n");
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
        drawWord("\nKERNEL PANIC: Ran out of memory!\n");
        return NULL;
    }

    for (uintptr_t i = page_index; i < page_index + pages_needed; i++){
        heap_struct.pages[i] = ALLOCATED; // maybe we should reservate the memory with the pid of the process that is using it
    }

    return (void*)( heap_struct.start + page_index * heap_struct.page_size);
}


// this free only frees one page: the one corresponding to the memory address passed as parameter
void * free(void * mem){
    uintptr_t page_index = get_page_index(mem);
    if(page_index==INVALID_ADDRESS){
        return NULL;
    }
    heap_struct.pages[page_index] = FREE;
    heap_struct.last_freed_page = page_index;
    return mem+heap_struct.page_size;
}

static uintptr_t get_page_index(void * mem){
    if((uintptr_t)mem > heap_struct.end || (uintptr_t) mem < heap_struct.start){
        drawWord("\nKERNEL PANIC: Invalid address!\n");
        return INVALID_ADDRESS;
    }
    return ((uintptr_t)mem - heap_struct.start) / heap_struct.page_size;
}

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define YELLOW 0xFFFF00
#define ORANGE 0xFFA500

static const int color_list[7] = { RED,GREEN,BLUE,WHITE,BLACK,YELLOW,ORANGE};
void printMem(){
    int c = 0;
    uintptr_t i=0;
    for (i = 0; i < heap_struct.page_qty; i++){
        if (heap_struct.pages[i] == 0)
            drawNumberColor(heap_struct.pages[i], RED);
        else{
            drawNumber(heap_struct.pages[i]);
            if (heap_struct.pages[i] <= 10)
                c++;
        }
        drawWord("|");
    }
    drawWord("\n");
}
