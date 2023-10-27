#include <circular_list.h>
#include <memory_manager.h>

typedef node_t * node_ptr;

node_ptr create_node(void * data){
    node_ptr new_node = (node_ptr) malloc(sizeof(node_t));
    if(new_node == NULL){
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void insert_node(queue_t * queue, node_t * new_node){
    if(queue == NULL || new_node == NULL){
        return;
    }
    if(queue->first == NULL){
        queue->first = new_node;
        queue->current_node = new_node;
        return;
    }
    queue->current_node->next = new_node;
    queue->current_node = new_node;
    new_node->next = queue->first;
    return;
}


node_ptr next(node_ptr node){
    if(node == NULL){
        return NULL;
    }
    return node->next;
}

void remove_node(node_ptr first, node_ptr node){
    if(first == NULL || node == NULL){
        return;
    }
    node_ptr current_node = first;
    while(current_node->next != node){
        current_node = current_node->next;
    }
    current_node->next = node->next;
    free(node);
}
