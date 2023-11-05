#include "include/memory_manager.h"
#include "./drivers/include/videoDriver.h"
#include<stdio.h>

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
    drawWord("Error: Not enough memory available");
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

Node * findNodeByIndex(Node * node,int index){
    
    if (node->index == index)
    {
        return node;
    }
    else
    {
        if (node->left != NULL)
        {
            Node * foundNode = findNodeByIndex(node->left,index);
            if (foundNode != NULL)
            {
                return foundNode;
            }
        }
        if (node->right != NULL)
        {
            Node * foundNode = findNodeByIndex(node->right,index);
            if (foundNode != NULL)
            {
                return foundNode;
            }
        }
        return NULL;
    }
}


void printMem(){
    drawWord("Total memory:");
    drawNumber(root->size );
    drawWord("\n");
    drawWord("Free memory:");
    drawNumber(root->size - memAllocated);
    drawWord("\n");
    drawWord("Allocated memory:");
    drawNumber(memAllocated);
    drawWord("\n");

}

// Creo que debería se void, porque no tendría sentido devolver un puntero a un espacio de memoria que ya no existe.
// Para lo único que se me ocurre es para que sepas si se liberó o no, pero eso lo podés saber por el estado del nodo.
void * free(void * mem){
    return free_rec(root,mem);
}

void * free_rec(Node * node, void * ptr){
    if (node->start == ptr)
    {
        node->state = FREE;
        Node * parent = findNodeByIndex(root,(node->index-1)/2);
        if(parent != NULL){
            if (parent->left->state == FREE && parent->right->state == FREE)
            {
                parent->state = FREE;
            }
            else
            {
                parent->state = PARCIAL;
            }
            
        }
        return node->start;
    }
    if (node->left != NULL){
        free_rec(node->left,ptr);
    }
    if(node->right != NULL){
        free_rec(node->right,ptr);
    }
    return NULL;
}