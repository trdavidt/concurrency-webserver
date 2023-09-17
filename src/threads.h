#ifndef THREADS_H
#define THREADS_H

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

#define Pthread_create(thread, attr, start_routine, arg) \
    assert(pthread_create(thread, attr, start_routine, arg) == 0)
#define Pthread_join(thread, retval) \
    assert(pthread_join(thread, retval) == 0)

#define Pthread_mutex_init(mutex, val) \
    assert(pthread_mutex_init(mutex, val) == 0);
#define Pthread_mutex_lock(mutex) \
    assert(pthread_mutex_lock(mutex) == 0);
#define Pthread_mutex_unlock(mutex) \
    assert(pthread_mutex_unlock(mutex) == 0);

#define Pthread_cond_init(cond, val) \
    assert(pthread_cond_init(cond, val) == 0);
#define Pthread_cond_signal(cond) \
    assert(pthread_cond_signal(cond) == 0);
#define Pthread_cond_wait(cond, mutex) \
    assert(pthread_cond_wait(cond, mutex) == 0);

#define Mutex_init(mutex)\
    assert(pthread_mutex_init(mutex, NULL) == 0);
#define Mutex_lock(mutex)\
    assert(pthread_mutex_lock(mutex) == 0);
#define Mutex_unlock(mutex)\
    assert(pthread_mutex_unlock(mutex) == 0);

#define Cond_init(cond)\
    assert(pthread_cond_init(cond, NULL) == 0);
#define Cond_wait(cond, mutex)\
    assert(pthread_mutex_wait(cond, mutex) == 0);
#define Cond_signal(cond)\
    assert(pthread_mutex_signal(cond) == 0);

#endif
