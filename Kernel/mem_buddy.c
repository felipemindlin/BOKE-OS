#include<memory_manager.h>

Node * root;

void* malloc_rec(Node* root,uint64_t bytes);

Node* findNode(Node* root, uint64_t bytes);

void createChilds(Node * node);

void printTree(Node* node);


void createMemoryManager(void * baseAddres, uint64_t mem_ammount){
    root = (Node*)baseAddres;
    root->start = baseAddres;

    root->size = mem_ammount;

    root->state = FREE;
    root->index = 0;
    root->left= NULL;
    root->right= NULL; 
}

void * malloc1(uint64_t bytes){
    if (root->size < bytes)
    {
        return NULL;
    }
    else{
        return (malloc_rec(root,bytes));
    }
}

void * malloc_rec(Node* node,uint64_t bytes){
    Node* foundNode = findNode(node,bytes);
    if (foundNode != NULL)
    {
        foundNode->state = ALLOCATED;
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

Node* findNode(Node* node, uint64_t bytes) {
    if (node->start == NULL) {
        return NULL;
    }
    if (node->size == bytes && node->state == FREE) {
        return node;
    }
    if (node->left != NULL){
        Node* foundNode = findNode(node->left, bytes);
        if (foundNode != NULL) {
            return foundNode;
        }  
    }
    if(node->right != NULL){
        Node* foundNode = findNode(node->right, bytes);
        if (foundNode != NULL) {
            return foundNode;
        }  
    }
    return NULL; 
}
/*
void * free(void * mem){
    Node* node = (Node*)mem;
    node->state = FREE;
    return mem;
}

void printTree(Node* node){
    if (node->start == NULL) {
        return;
    }
    printTree(node->left);
    printTree(node->right);
}

void printMem(){
    printTree(root);
}
*/
