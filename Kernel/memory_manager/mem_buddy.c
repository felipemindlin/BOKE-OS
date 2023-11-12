#include "include/memory_manager.h"
#include "./drivers/include/videoDriver.h"
#include <stdio.h>

#define IS_POWER_OF_TWO(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define MIN_BLOCK 16

Node * root;
uint64_t memAllocated = 0;

void* malloc_rec(Node* node,uint64_t bytes);
void createChilds(Node * node);
void * free_rec(Node * node, void * ptr);

void init_mm(void * baseAddres, uint64_t mem_ammount){
    root = (Node *) baseAddres;
    root->start =  (void*)mem_ammount;
    root->size = mem_ammount;
    root->state = FREE;
    root->index = 0;
    root->left= NULL;
    root->right= NULL; 
}

void checkState(Node* node){
    if (node->left == NULL || node->right == NULL)
    {   
        node->state = FREE; 
        return;
    }
    
    if (node->right->state == FREE && node->left->state == FREE)
        {
            node->state = FREE;
        }

    else if (node->left->state == ALLOCATED && node->right->state == ALLOCATED)
        {
            node->state = ALLOCATED;
        }
    else
        {
            node->state = PARCIAL;
        }
}

void * malloc(uint64_t bytes){
if (root->size < bytes){
    draw_word("Error: Not enough memory available");
    return NULL;
}

if (!IS_POWER_OF_TWO(bytes)){
    int i = 1;
    while (i < bytes){
        i = i*2;
    }
    bytes = i;
}
    void * rta = malloc_rec(root,bytes);  
    return rta;
}

void * malloc_rec(Node* node,uint64_t bytes){    
    if (node->state == ALLOCATED){
        return NULL;
    }
   if(node->left != NULL || node->right != NULL){
    void * newNode = malloc_rec(node->left,bytes);
    if (newNode == NULL){
        newNode = malloc_rec(node->right,bytes);
    }
    checkState(node);
    return newNode;
   }
   else{
    if (bytes > node->size)
    {
        return NULL;
    }
    
    if((node->size)/2 >= bytes){
        createChilds(node);
        void * newNode = malloc_rec(node->left,bytes);
        checkState(node);
        return newNode;
    }
    node->state = ALLOCATED;
    memAllocated += bytes;
    return node->start;
   }
}

void createChilds(Node * node){
    uint64_t parentIndex = node->index;
    uint64_t leftIndex = parentIndex*2 + 1;
    uint64_t rightIndex = leftIndex + 1;
    uint64_t newSize = (node->size)/2;

    node->left = node + leftIndex;
    node->left->start = node->start;
    node->left->size = newSize;
    node->left->state = FREE;
    node->left->index = leftIndex;

    node->right = node + rightIndex;
    node->right->start = (void*)((char*) (node->start) + newSize);
    node->right->size = newSize;
    node->right->state = FREE;
    node->right->index = rightIndex;

}

void printMem(){
    draw_word("Total memory:");
    drawNumber(root->size );
    draw_word("\n");
    draw_word("Free memory:");
    drawNumber(root->size - memAllocated);
    draw_word("\n");
    draw_word("Allocated memory:");
    drawNumber(memAllocated);
    draw_word("\n");
}

// Creo que debería se void, porque no tendría sentido devolver un puntero a un espacio de memoria que ya no existe.
// Para lo único que se me ocurre es para que sepas si se liberó o no, pero eso lo podés saber por el estado del nodo.
void free(void * mem){
    free_rec(root,mem);
}

void * free_rec(Node * node, void * ptr){
    if(node == NULL){
        return NULL;
    }

    if (node->left != NULL || node->right != NULL){
        if ((uint64_t)node->right->start > (uint64_t) ptr){
            free_rec(node->left,ptr);
        }
        else{
            free_rec(node->right,ptr);
        }
        checkState(node);
        if (node->state == FREE)
        {
            node->left = NULL;
            node->right = NULL;
        }
    }
    else if (node->state == ALLOCATED){
        if (node->start == ptr)
        {
            node->state = FREE;
            memAllocated -= node->size;
        }
    }
    
    return NULL;

}