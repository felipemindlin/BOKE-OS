

#define MAX 1001
#define START 0
#define END 1000
#define BLOCK_SIZE 10

struct Heap {
    uintptr_t start;
    uintptr_t end;
    uintptr_t blocks;
    uintptr_t block_size;
    int *arr;
};

// static const uintptr_t start = 0;
// static const uintptr_t end = 1000;
// static const uintptr_t blocks = 100;
// static const uintptr_t block_size = ((end-start)/blocks);

int arr[MAX];
struct Heap heap = {START,END, (END-START)/BLOCK_SIZE,BLOCK_SIZE, arr};
typedef struct Heap * Heap_ptr;
Heap_ptr heap_ptr = &heap;

void initializeHeap(){
    for (int i = 0; i < MAX; i++){
        heap_ptr->arr[i] = -1;
    }
     for (int i = 0; i < heap_ptr->blocks; i++){
        heap_ptr->arr[i] = 0;
    }
}

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

uintptr_t free(void * mem){
    uintptr_t c = 0;
    uintptr_t flag_found = 0;
    while (heap_ptr->arr[c] != -1){
        if (heap_ptr->arr[c] != 0){
            if ((void*)(c*heap_ptr->block_size) <= mem && 
                (void*)(c*heap_ptr->block_size + heap_ptr->block_size) > mem )
                    flag_found = 1;
            if (flag_found){
                if (heap_ptr->arr[c] <= 10){
                   heap_ptr->arr[c] = 0;
                   return 0; 
                }
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