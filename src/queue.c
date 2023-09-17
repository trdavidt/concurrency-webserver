#include "common.h"

queue *init_queue(int max_size) {
   
   // Initialize queue
   queue *q = (queue *)malloc(sizeof(queue));
   if (!q) exit(1);
   q->curr_size = 0;
   q->max_size = max_size;
   q->fill = 0;
   q->use = 0;

   // Initialize nodes
   q->nodes = (node *)malloc(sizeof(node)*max_size);
   if (!q->nodes) exit(1);
   for (int i = 0; i < max_size; i++) {
        q->nodes[i].fd = 0;
    }

    return q;
}

// Insert node into queue (circular buffer)
void queue_insert(queue *q, int sockfd) {
    
    assert(q->curr_size < q->max_size);

    // Insert at fill index
    q->nodes[q->fill].fd = sockfd;
    q->fill = (q->fill+1) % q->max_size;
    q->curr_size++;
}

// Remove from front of queue
int queue_remove(queue *q) {
    
    assert(q->curr_size > 0);

    // Remove at use index
    int sockfd = q->nodes[q->use].fd;
    q->use = (q->use+1) % q->max_size;
    q->curr_size--;

    return sockfd;
}

