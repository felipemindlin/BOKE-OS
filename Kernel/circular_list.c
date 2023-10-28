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
    new_node->prev = NULL;
    return new_node;
}

void insert_node(queue_t * queue, node_t * new_node){
    if(queue == NULL || new_node == NULL){
        return;
    }
    if(queue->first == NULL){
        queue->first = new_node;
        queue->current_node = new_node;
        queue->first->next = queue->first;
        queue->first->prev = queue->first;
        return;
    }
    new_node->prev = queue->current_node;
    new_node->next = queue->current_node->next;
    queue->current_node->next = new_node;
    queue->current_node = new_node;
    new_node->next->prev = new_node;
    return;
}


node_ptr next(queue_t * queue, node_ptr node){
    if(node == NULL){
        return NULL;
    }
    if(node->next == NULL){
        return queue->first;
    }
    return node->next;
}

void remove_node(queue_t * queue, node_ptr node){
    if(node == NULL){
        return;
    }
    if(node == queue->first){
        queue->first = node->next;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
    return;
}
