#include "common.h"

scheduler *init_scheduler(char *policy, int buffers) {
    
    // Initialize scheduler
    scheduler *s = (scheduler *)malloc(sizeof(scheduler));
    if (!s) return NULL;
    s->policy = policy;
    s->buffers = buffers;
    s->curr_size = 0;

    // Alloc data structures based on policy (SFF or FIFO)
    if (strcmp(SFF, policy) == 0) {
        s->h = init_heap(buffers);
        s->q = NULL;
    }
    else if (strcmp(FIFO, policy) == 0) {
        s->h = NULL;
        s->q = init_queue(buffers);
    }
    return s;
}

thread_pool *init_thread_pool(int num_threads) {
    
    // Initialize thread pool
    thread_pool *workers = (thread_pool *)malloc(sizeof(thread_pool));
    if (!workers) return NULL;
    workers->num_threads = num_threads;
    workers->working_threads = 0;
    workers->pool = malloc(num_threads*sizeof(pthread_t));

    // Intiialize mutexes and condiiton vars
    Pthread_mutex_init(&workers->lock, NULL);
    Pthread_cond_init(&workers->full, NULL);
    Pthread_cond_init(&workers->empty, NULL);

    return workers;
}

// Receive socket fd from scheduler and give to request handler
void *worker_routine(void *arg) {
    worker_arg *args = (worker_arg *)arg;
    while(1) {
        int sockfd = scheduler_get(args->workers, args->scheduler);
        request_handle(sockfd);
        close_or_die(sockfd);
    }
    return NULL;
}

// Create worker threads and invoke worker_routine on each
void create_workers(scheduler *s, thread_pool *workers) {

    for (int i = 0; i < workers->num_threads; i++) {
        worker_arg *arg = (worker_arg *)malloc(sizeof(worker_arg));
        if (!arg) {
            printf("create_workers: failed to create thread %d\n", i);
            continue;
        }
        arg->workers = workers;
        arg->scheduler = s;
        arg->worker_index = i;
        Pthread_create(&workers->pool[i], NULL, worker_routine, arg);
    }
}

// Used internally, add request to scheduler data structure
void scheduler_new_req(scheduler *s, int sockfd) {

    if (strcmp(s->policy, SFF) == 0) {
        off_t fsize = get_fsize(sockfd);
               heap_insert(s->h, sockfd, fsize);
    } 
    else if (strcmp(FIFO, s->policy) == 0) {
       queue_insert (s->q, sockfd);
    }
    s->curr_size++;
}

// Used internally, returns next scheduled request
int scheduler_pick_req(scheduler *s) {

    int sockfd;
    if (strcmp(s->policy, SFF) == 0) {
        sockfd = extract_min(s->h);
    }
    else if (strcmp(s->policy, FIFO) == 0) {
        sockfd = queue_remove(s->q);
    }
    s->curr_size--;
    return sockfd;
}

// Is scheduler full?
int scheduler_is_full(scheduler *s) {
    return s->curr_size == s->buffers;
}

// Is scheduler empty?
int scheduler_is_empty(scheduler *s) {
    return s->curr_size == 0;
}

// Pass a new request to the scheduler, blocks calling thread
void scheduler_post(thread_pool *workers, scheduler *s, int sockfd) {

    Pthread_mutex_lock(&workers->lock);

    while (scheduler_is_full(s)) {
        Pthread_cond_wait(&workers->full, &workers->lock);
    }
    scheduler_new_req(s, sockfd);
    Pthread_cond_signal(&workers->empty);

    Pthread_mutex_unlock(&workers->lock);
}

// Retrieve request from scheduler, blocks calling thread
int scheduler_get(thread_pool *workers, scheduler *s) {
    
    Pthread_mutex_lock(&workers->lock);

    while (scheduler_is_empty(s)) {
        Pthread_cond_wait(&workers->empty, &workers->lock);
    }
    int sockfd = scheduler_pick_req(s);
    Pthread_cond_signal(&workers->full);
    
    Pthread_mutex_unlock(&workers->lock);

    return sockfd;
}


