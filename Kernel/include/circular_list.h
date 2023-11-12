#ifndef _CIRCULAR_LIST_H_
#define _CIRCULAR_LIST_H_

typedef struct node_t {
	void * data;
	struct node_t * next;
	struct node_t * prev;
} node_t;

typedef struct queue_t {
	node_t * current_node;
	unsigned int qty;
} queue_t;

#include <process.h>

node_t * create_node(void * data);
int insert_node(queue_t * queue, node_t * new_node);
node_t * next(node_t * node);
void * remove_node(queue_t * queue, node_t * node);
queue_t * create_queue();
														
#endif