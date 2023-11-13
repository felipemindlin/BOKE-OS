#include "include/memory_manager.h"
#include "./drivers/include/videoDriver.h"
#include <stdio.h>

#define IS_POWER_OF_TWO(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define MIN_BLOCK 16

Node * root;
uint64_t mem_allocated = 0;

void* malloc_rec(Node* node, uint64_t bytes);
void create_childs(Node * node);
void * free_rec(Node * node, void * ptr);

void init_mm(void * base_addres, uint64_t mem_ammount){
    root = (Node *) base_addres;
    root->start =  (void*)mem_ammount;
    root->size = mem_ammount;
    root->state = FREE;
    root->index = 0;
    root->left= NULL;
    root->right= NULL; 
}

void check_state(Node* node){
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
    void * rta = malloc_rec(root, bytes);  
    return rta;
}

void * malloc_rec(Node* node, uint64_t bytes){    
    if (node->state == ALLOCATED){
        return NULL;
    }
   if(node->left != NULL || node->right != NULL){
    void * new_node = malloc_rec(node->left, bytes);
    if (new_node == NULL){
        new_node = malloc_rec(node->right, bytes);
    }
    check_state(node);
    return new_node;
   }
   else{
    if (bytes > node->size)
    {
        return NULL;
    }
    
    if((node->size)/2 >= bytes){
        create_childs(node);
        void * new_node = malloc_rec(node->left, bytes);
        check_state(node);
        return new_node;
    }
    node->state = ALLOCATED;
    mem_allocated += bytes;
    return node->start;
   }
}

void create_childs(Node * node){
    uint64_t parent_index = node->index;
    uint64_t left_index = parent_index*2 + 1;
    uint64_t right_index = left_index + 1;
    uint64_t new_size = (node->size)/2;

    node->left = node + left_index;
    node->left->start = node->start;
    node->left->size = new_size;
    node->left->state = FREE;
    node->left->index = left_index;

    node->right = node + right_index;
    node->right->start = (void*)((char*) (node->start) + new_size);
    node->right->size = new_size;
    node->right->state = FREE;
    node->right->index = right_index;

}

void print_mem(){
    draw_word("Total memory:");
    draw_number(root->size );
    draw_word("\n");
    draw_word("Free memory:");
    draw_number(root->size - mem_allocated);
    draw_word("\n");
    draw_word("Allocated memory:");
    draw_number(mem_allocated);
    draw_word("\n");
}

void free(void * mem){
    free_rec(root, mem);
}

void * free_rec(Node * node, void * ptr){
    if(node == NULL){
        return NULL;
    }

    if (node->left != NULL || node->right != NULL){
        if ((uint64_t)node->right->start > (uint64_t) ptr){
            free_rec(node->left, ptr);
        }
        else{
            free_rec(node->right, ptr);
        }
        check_state(node);
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
            mem_allocated -= node->size;
        }
    }
    
    return NULL;

}