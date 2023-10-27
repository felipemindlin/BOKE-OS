#ifndef _CIRCULAR_LIST_H_
#define _CIRCULAR_LIST_H_

typedef struct node_t {
	void * data;
	struct node_t * next;
	struct node_t * prev;
} node_t;

typedef struct queue_t {
	node_t * first;
	node_t * current_node;
	unsigned int process_qty;
} queue_t;

node_t * create_node(void * data);
void insert_node(queue_t * queue, node_t * new_node);
node_t * next(node_t * node);
void remove_node(node_t * first, node_t * node);


#endif