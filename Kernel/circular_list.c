#include <circular_list.h>
#include <memory_manager.h>

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
    // we insert the node at the end of the queue
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

void remove_node(queue_t * queue, node_ptr node){
    if(node == NULL){
        return;
    }
    if(queue->current_node == node){
        queue->current_node = node->next;
    }
    node_ptr prev_node = node->prev;
    node_ptr next_node = node->next;
    prev_node->next = next_node;
    next_node->prev = prev_node;
    free(node);
    queue->qty--;
}