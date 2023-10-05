#include <memory_manager.h>

#define MAX 1001
#define START 0
#define END 1000
#define BLOCK_SIZE 10

typedef struct heap {
    uintptr_t start;
    uintptr_t end;
    uintptr_t blocks;
    uintptr_t block_size;
    uintptr_t last_freed_page;
    int *arr;
} heap;

int arr[MAX]={0};
heap heap_info_structure = {START,END, (END-START)/BLOCK_SIZE,BLOCK_SIZE, 0, arr};
heap * heap_ptr = &heap_info_structure;

void initializeHeap(){
    for (int i = 0; i < MAX; i++){
        heap_ptr->arr[i] = -1;
    }
     for (int i = 0; i < heap_ptr->blocks; i++){
        heap_ptr->arr[i] = 0;
    }
}

static size_t size_to_num_of_pages(size_t size){
    size_t pages_needed = size / heap_ptr->block_size;
    if (size % heap_ptr->block_size != 0){
        pages_needed++;
    }
    return pages_needed;
}

uintptr_t find_pages(size_t pages_needed, uintptr_t start_page){
    uintptr_t page_index = 0;
    uintptr_t pages_found = 0;
    for (uintptr_t i = start_page; i < heap_ptr->blocks; i++){
        if (heap_ptr->arr[i] == 0){
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

void* malloc(uintptr_t size){
    size_t pages_needed = size_to_num_of_pages(size);
    uintptr_t page_index = find_pages(pages_needed, heap_ptr->last_freed_page);
    if(page_index==INVALID_ADDRESS){
        page_index = find_pages(pages_needed, 0);
    }
    if(page_index==INVALID_ADDRESS){
        drawWord("\nKERNEL PANIC: Ran out of memory!\n");
        return NULL;
    }

    for (uintptr_t i = page_index; i < page_index + pages_needed; i++){
        heap_ptr->arr[i] = (char) size; // maybe we should reservate the memory with the pid of the process that is using it
    }

    return (void*)(page_index * heap_ptr->block_size);
}

/*
void* malloc(uintptr_t bytes){
    uintptr_t c = 0;
    uintptr_t d = 0;
    uintptr_t k = 0;
    uintptr_t i = bytes;
    uintptr_t enough_contig_mem_flag = 0;
    if (bytes <= 0 || bytes >= MAX*heap_ptr->block_size){
        return (void*)-1;
    }
    while (heap_ptr->arr[c] != -1){
        if (heap_ptr->arr[c] == 0){
            k=c;
            enough_contig_mem_flag = 1;
            while (k <= c +((bytes-1)/heap_ptr->block_size) && enough_contig_mem_flag){
                if (heap_ptr->arr[k] != 0){
                    enough_contig_mem_flag = 0;
                    c=k+1;
                }
                k++;
            }
            if (enough_contig_mem_flag){
            d = c;
            while (d <= c +(bytes/heap_ptr->block_size) ) {
                heap_ptr->arr[d] = i;
                i-=heap_ptr->block_size;
                d++;
            }
            return (void*)(c*heap_ptr->block_size);
            }
        }
        c++;
    }
    return (void*)-1;
}
*/

uintptr_t free(void * mem){
    uintptr_t c = 0;
    uintptr_t flag_found = 0;
    while (heap_ptr->arr[c] != -1 && c<heap_ptr->blocks){
        if (heap_ptr->arr[c] != 0){
            if ((void*)(c*heap_ptr->block_size) <= mem && 
                (void*)(c*heap_ptr->block_size + heap_ptr->block_size) > mem )
                    flag_found = 1;
            if (flag_found){
                // for (int i = 0; i < heap_ptr->block_size; i++){
                //     ((uint8_t*)(c*heap_ptr->block_size))[i]=0;
                // }
                /*if (heap_ptr->arr[c] <= 10){
                   heap_ptr->arr[c] = 0;
                   return 0; 
                }*/
                heap_ptr->arr[c] = 0;
            }
            }
        c++;
    }
    return -1;
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
    for (uintptr_t i = 0; i < heap_ptr->blocks; i++){
        if (heap_ptr->arr[i] == 0)
            drawNumber(heap_ptr->arr[i]);
        else{
            drawNumberColor(heap_ptr->arr[i], color_list[c]);
            if (heap_ptr->arr[i] <= 10)
                c++;
            }
        drawWord("|");
    }
}