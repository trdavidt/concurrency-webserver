#include "common.h"

// Compares nodes based on file size (SFF)
int heap_comparator(heap *h, int i, int j) {
    if (h->nodes[i].value > h->nodes[j].value) {
       return 1;
    }
    return 0;
}

// Swap two nodes
void swap(node *m, node *n) {
    node temp = *m;
    *m = *n;
    *n = temp;
}

heap *init_heap(int max_size) {

    // Initialize heap
    heap *h = (heap *)malloc(sizeof(heap));
    if (!h) exit(1);
    h->max_size = max_size;
    h->curr_size = 0;
    
    // Initialize nodes
    h->nodes = (node *)malloc(max_size*sizeof(node));
    if (!h->nodes) exit(1);
    for (int i = 0; i < max_size; i++) {
        h->nodes[i].fd = 0;
        h->nodes[i].value = 0;
    }

    return h;
}

// Insert node into heap
void heap_insert(heap *h, int sockfd, off_t value) {

    assert(h->curr_size < h->max_size);
    
    // Insert at last position in nodes array
    int index = h->curr_size;
    h->curr_size++;
    h->nodes[index].fd = sockfd; h->nodes[index].value = value;

    // Heapify from bottom up until heap is min-heap
    while (index != 0 && (heap_comparator(h, index/2, index) == 1)) {
        swap(&h->nodes[index/2], &h->nodes[index]);
        index /= 2;
    }
}

// Heapify from top down from given node
void heapify(heap *h, int index) {

    // Left and right children
    int left = 2*index + 1;
    int right = 2*index + 2;

    // Find index of smallest child 
    int smallest = index;
    if (left < h->curr_size && heap_comparator(h, smallest, left) == 1) {
        smallest = left;
    }
    if (right < h->curr_size && heap_comparator(h, smallest, right) == 1) {
        smallest = right;
    }

    // Swap curr node with smallest child
    if (smallest != index) {
        swap(&h->nodes[smallest], &h->nodes[index]);
        heapify(h, smallest);
    }

}

// Remove minimum/root node and returns fd
int extract_min(heap *h) {
    
    assert(h->curr_size > 0);

    // "Replace" root with last node since heap must be complete bin tree
    swap(&h->nodes[0], &h->nodes[h->curr_size-1]);
    int fd = h->nodes[h->curr_size-1].fd;
    h->curr_size--;

    // Heapify from root down
    heapify(h, 0);
    return fd;
}

