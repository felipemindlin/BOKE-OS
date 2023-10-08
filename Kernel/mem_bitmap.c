#include <memory_manager.h>

#define MAX 1001
#define PAGE_SIZE 16

heap heap_struct;

uintptr_t get_page_index(void * mem);
void initialize_pages();
uintptr_t find_pages(size_t pages_needed, uintptr_t start_page);
static size_t size_to_num_of_pages(size_t size);

void initialize_heap(void * baseAddres, uint64_t mem_ammount){
    heap_struct.start = (uintptr_t)baseAddres;
    heap_struct.size = mem_ammount;
    heap_struct.end = heap_struct.start + heap_struct.size;
    heap_struct.page_size = PAGE_SIZE;
    heap_struct.page_qty = heap_struct.size / heap_struct.page_size;
    heap_struct.last_freed_page = 0;
    heap_struct.pages = (uint8_t *)heap_struct.start;
    initialize_pages();
}

void initialize_pages(){
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

uintptr_t find_pages(size_t pages_needed, uintptr_t start_page){
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
        heap_struct.pages[i] = (char) ALLOCATED; // maybe we should reservate the memory with the pid of the process that is using it
    }

    return (void*)(page_index * heap_struct.page_size);
}

/*
void* malloc(uintptr_t bytes){
    uintptr_t c = 0;
    uintptr_t d = 0;
    uintptr_t k = 0;
    uintptr_t i = bytes;
    uintptr_t enough_contig_mem_flag = 0;
    if (bytes <= 0 || bytes >= MAX*heap_struct.page_size){
        return (void*)-1;
    }
    while (heap_struct.pages[c] != -1){
        if (heap_struct.pages[c] == 0){
            k=c;
            enough_contig_mem_flag = 1;
            while (k <= c +((bytes-1)/heap_struct.page_size) && enough_contig_mem_flag){
                if (heap_struct.pages[k] != 0){
                    enough_contig_mem_flag = 0;
                    c=k+1;
                }
                k++;
            }
            if (enough_contig_mem_flag){
            d = c;
            while (d <= c +(bytes/heap_struct.page_size) ) {
                heap_struct.pages[d] = i;
                i-=heap_struct.page_size;
                d++;
            }
            return (void*)(c*heap_struct.page_size);
            }
        }
        c++;
    }
    return (void*)-1;
}


uintptr_t free(void * mem){
    uintptr_t c = 0;
    uintptr_t flag_found = 0;
    while (heap_struct.pages[c] != ALLOCATED && c<heap_struct.page_qty){
        if (heap_struct.pages[c] != 0){
            if ((void*)(c*heap_struct.page_size) <= mem && 
                (void*)(c*heap_struct.page_size + heap_struct.page_size) > mem )
                    flag_found = 1;
            if (flag_found){
                // for (int i = 0; i < heap_struct.page_size; i++){
                //     ((uint8_t*)(c*heap_struct.page_size))[i]=0;
                // }
                if (heap_struct.pages[c] <= 10){
                   heap_struct.pages[c] = 0;
                   return 0; 
                }
                heap_struct.pages[c] = 0;
            }
            }
        c++;
    }
    return -1;
}
*/

uintptr_t free(void * mem){
    uintptr_t c = 0;
    uintptr_t flag_found = 0;
    uintptr_t page_index = get_page_index(mem);
    uintptr_t pages_to_free = size_to_num_of_pages(heap_struct.pages[page_index]);
    for (uintptr_t i = page_index; i < page_index + pages_to_free; i++){
        heap_struct.pages[i] = FREE;
    }
    return 0;
}

uintptr_t get_page_index(void * mem){
    uintptr_t page_index = (uintptr_t)mem / heap_struct.page_size;
    return page_index;
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
    for (uintptr_t i = 0; i < heap_struct.page_qty; i++){
        if (heap_struct.pages[i] == 0)
            drawNumber(heap_struct.pages[i]);
        else{
            drawNumberColor(heap_struct.pages[i], color_list[c]);
            if (heap_struct.pages[i] <= 10)
                c++;
            }
        drawWord("|");
    }
}