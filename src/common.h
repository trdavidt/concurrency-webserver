#ifndef COMMON_H 
#define COMMON_H 

#include "threads.h"
#include "request.h"
#include "io_helper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

// Constants
#define MAXBUF 16384
#define MAXFILETYPE 8192
#define FIFO "FIFO"
#define SFF "SFF"

// Default wserver args
#define DEFAULT_ROOT "."
#define DEFAULT_PORT 10000
#define DEFAULT_THREADS 1
#define DEFAULT_BUFFERS 1
#define DEFAULT_POLICY "FIFO"

// Node for heap and queue
typedef struct __node_t {
  int fd;
  off_t value;
  char *fname;
} node;

// Binary heap
typedef struct __heap_t {
  int max_size;
  int curr_size;
  node *nodes;
} heap;

// Heap methods
heap *init_heap(int max_size);
void heap_insert(heap *h, int sockfd, off_t value);
int heap_comparator(heap *h, int i, int j);
void heapify(heap *h, int index);
void swap(node *m, node *n);
int extract_min(heap *h);

// Queue
typedef struct __queue_t {
  int max_size;
  int curr_size;
  int fill;
  int use;
  node *nodes;
} queue;

// Queue methods
queue *init_queue(int max_size);
void queue_insert(queue *q, int sockfd);
int queue_remove(queue *q);

// Thread pool
typedef struct __thread_pool_t {
  int num_threads;
  int working_threads;
  pthread_t *pool;
  pthread_mutex_t lock;
  pthread_cond_t full;
  pthread_cond_t empty;
} thread_pool;

// Scheduler
typedef struct __scheduler_t {
  char *policy;
  int buffers;
  int curr_size;
  heap *h;
  queue *q;
} scheduler;

// Scheduler methods
scheduler *init_scheduler(char *policy, int buffers);
thread_pool *init_thread_pool(int num_threads);
void create_workers(scheduler *s, thread_pool *workers);
void scheduler_new_req(scheduler *s, int sockfd);
int scheduler_pick_req(scheduler *s);
int scheduler_is_full(scheduler *s);
int scheduler_is_empty(scheduler *s);
void scheduler_post(thread_pool *workers, scheduler *s, int sockfd);
int scheduler_get(thread_pool *workers, scheduler *s);

// Args passed to worker threads
typedef struct __worker_arg_t {
  int worker_index;
  scheduler *scheduler;
  thread_pool *workers;
} worker_arg;

// Worker thread routine
void *worker_routine(void *arg);

// File size from socket fd
off_t get_fsize(int sockfd);

// Args passed to client threads
typedef struct __client_arg_t {
  char *host;
  char *fname;
  int port;
} client_arg;

// Client thread routine
void *client_routine(void *arg);

#endif
