#include <circular_list.h>
#include "../memory_manager/include/memory_manager.h"

typedef node_t * node_ptr;

queue_t * create_queue(){
    queue_t * queue = (queue_t *) malloc(sizeof(queue_t));
    if(queue == NULL){
        return NULL;
    }
    queue->current_node = NULL;
    queue->qty = 0;
    return queue;
}

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

int insert_node(queue_t * queue, node_t * new_node){
    if(new_node == NULL){
        return -1;
    }
    if(queue->current_node == NULL){
        queue->current_node = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        return queue->qty++;
    }
        new_node->next = queue->current_node;
    node_ptr last_node = queue->current_node->prev;
    new_node->prev = last_node;
    last_node->next = new_node;
    queue->current_node->prev = new_node;
    return queue->qty++;
}


node_ptr next(node_ptr node){
    if(node == NULL){
        return NULL;
    }
    return node->next;
}

void * remove_node(queue_t * queue, node_ptr node){     if(queue == NULL || node == NULL){
        return NULL;
    }
    if(queue->current_node == NULL){
        return NULL;
    }
    if(queue->current_node == node){
        queue->current_node = node->next;
    }
    if(queue->qty == 1){
        void * data = node->data;
        queue->current_node = NULL;
        free(node);
        queue->qty--;
        return data;
    }
    node_ptr prev_node = node->prev;
    node_ptr next_node = node->next;
    prev_node->next = next_node;
    next_node->prev = prev_node;
    void * data = node->data;
    free(node);
    queue->qty--;
    return data;
}

void free_queue(queue_t * queue){
    if(queue == NULL){
        return;
    }
    node_ptr current_node = queue->current_node;
    if (current_node != NULL){
        node_ptr next_node = current_node->next;
        while(current_node != next_node){
            node_ptr aux = next_node->next;
            free(next_node);
            next_node = aux;
        }
        free(current_node); 
    free(queue);
    }
}
