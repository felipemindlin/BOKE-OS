#include "include/memory_manager.h"
#include "./drivers/include/videoDriver.h"

#define IS_POWER_OF_TWO(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

Node * root;

void* malloc_rec(Node* root,uint64_t bytes);

Node* findNode(Node* root, uint64_t bytes);

void createChilds(Node * node);

void printTree(Node* node);

void * free_rec(Node * node, void * ptr);

void init_mm(void * baseAddres, uint64_t mem_ammount);
void * malloc(uintptr_t bytes);
void * free(void * mem);
void printMem();

void init_mm(void * baseAddres, uint64_t mem_ammount){
    root = (Node*)baseAddres;
    root->start = baseAddres;

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
if (root->size < bytes)
    {
        return NULL;
    }
    else{
        if (!IS_POWER_OF_TWO(bytes))
        {
            int i = 1;
            while (i < bytes)
            {
                i = i*2;
            }
            bytes = i;
        }
        
        return (malloc_rec(root,bytes));
    }
}

void * malloc_rec(Node* node,uint64_t bytes){
    Node* foundNode = findNode(node,bytes);
    if (foundNode != NULL)
    {
        foundNode->state = ALLOCATED;
        checkState(node); 
        return foundNode->start;
    }
    else
    {
        if (node->state == FREE)
        {
            createChilds(node);
        }
        void * new_node = malloc_rec(node->left,bytes);
        if (new_node != NULL)
        {
            checkState(node);
            return new_node;
        }
        else
        {
            return malloc_rec(node->right,bytes);
        }
    }
    
}

void createChilds(Node * node){
    uint64_t parentIndex = node->index;
    uint64_t leftIndex = parentIndex*2 + 1;
    uint64_t rightIndex = leftIndex + 1;

    node->left = node + leftIndex;
    node->left->start = node->start;
    node->left->size = node->size/2;
    node->left->state = FREE;
    node->left->index = leftIndex;

    node->right = node + rightIndex;
    node->right->start = (void*)((char*)node->start + node->size/2);
    node->right->size = node->size/2;
    node->right->state = FREE;
    node->right->index = rightIndex;
}

Node * findNodeByIndex(Node* node,int index){
    
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

Node* findNode(Node* node, uint64_t bytes) {
    if (node->start == NULL) {
        return NULL;
    }
    if ((node->size == bytes || node->size/2 < bytes) && node->state == FREE) {
        node->state = ALLOCATED;
        return node;
    }
    if (node->left != NULL){
        Node* foundNode = findNode(node->left, bytes);
        if (foundNode != NULL) {
             if(node->right->state == FREE){
                node->state = PARCIAL;
            }else{
                node->state = ALLOCATED;
            }
            return foundNode;
        }  
    }
    if(node->right != NULL){
        Node* foundNode = findNode(node->right, bytes);
        if (foundNode != NULL) {
            if (node->left->state == FREE)
            {
                node->state = PARCIAL;
            }else
            {
                node->state = ALLOCATED;
            }
            return foundNode;
        }  
    }
    return NULL; 
}

void stateMemory(Node* node,uint64_t * freeMemory,uint64_t * allocatedMemory){
 
    if (node->state == FREE)
    {
        *freeMemory = *freeMemory + node->size;
    }
    else if (node->state == ALLOCATED)
    {
        *allocatedMemory = *allocatedMemory + node->size;
    }
    if (node->left != NULL){
        stateMemory(node->left,freeMemory,allocatedMemory);
    }
    if(node->right != NULL){
        stateMemory(node->right,freeMemory,allocatedMemory);
    }
}

void showMemory(Node* node){
    drawWord("ShowMemory\n");
    drawWord("Memory:");
    drawWord(node->start);
    drawWord("\n");
    drawWord("Size:");
    drawNumber(node->size);
    drawWord("\n");
    drawWord("State:");
    if (node->state == FREE)
    {
        drawWord("FREE\n");
    }
    else if (node->state == ALLOCATED)
    {
        drawWord("ALLOCATED\n");
    }
    else if (node->state == PARCIAL)
    {
        drawWord("PARCIAL\n");
    }
    drawWord("Index:");
    drawNumber(node->index);
    drawWord("\n");
    drawWord("--------------------------------\n");
    if (node->left != NULL){
        showMemory(node->left);
    }
    if(node->right != NULL){
        showMemory(node->right);
    }
}


void printMem(){
    uint64_t freeMemory = 0;
    uint64_t allocatedMemory = 0;
    
    stateMemory(root,&freeMemory,&allocatedMemory);

    drawWord("Free memory:");
    drawNumber(freeMemory);
    drawWord("\n");
    drawWord("Allocated memory:");
    drawNumber(allocatedMemory);
    drawWord("\n");

    showMemory(root);
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
