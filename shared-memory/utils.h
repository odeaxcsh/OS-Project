#ifndef SHARED_MEMORY_UTILS_H_
#define SHARED_MEMORY_UTILS_H_

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

#define ERORR_BUFFER_SIZE 1024
#define BUFFER_LEN 8197

#define SERVER_REQUEST_SHARED_MEMROY_NAME "shared_memory_b"
#define SERVER_RESPONSE_SHARED_MEMORY_NAME "shared_memory_a"

#define SHARED_MEMORY_SIZE 8197
#define SHARED_MEMORY_SEGMENTS_NUM 8
#define MAX_NUMBER_OF_CLIENTS 8 

#define TRUE 1
#define FALSE 0
typedef unsigned char BOOL;

#define EXIT_REQUEST_MESSAGE "exit"

#define CONST_CAST(type, x) (*(type *)(&x))

// c++ `pointer to member dereferencing` alternative in c
// `RELATIVE_ACCESS(objectptr, pointer)` works like `objectptr->*pointer`
#define RELATIVE_ACCESS(objectptr, attrptr) (&(objectptr->attrptr))

#define tuple(type)             \
    struct {                    \
        type first;             \
        type second;            \
    }                           \

// new errors trigger
extern int error_cond;

// errors are written in  this buffer
extern char error_message[ERORR_BUFFER_SIZE];

// errors interface
#define get_warning() (error_cond ? NULL : error_message + (error_cond = 0)) 
#define set_warning(...) (error_cond ? 0 : sprintf(error_message, __VA_ARGS__) + (error_cond = 1))

#define fatal_error(...) {          \
    fprintf(stderr, __VA_ARGS__);   \
    fprintf(stderr, "\n");          \
    exit(EXIT_FAILURE);             \
}

// essential attributes for communication
struct memory_t
{
    // maximum number of available slots of memory
    const int size;

    // connection flags
    int list_head;
    int list_tail;

    sem_t items;
    sem_t empty;

    // main shared memory space
    char memory[SHARED_MEMORY_SEGMENTS_NUM][SHARED_MEMORY_SIZE];

    // memroy object access lock
    pthread_mutex_t push_lock;
    pthread_mutex_t fetch_lock;
};


// create and initialize a new memory object
void initialize(struct memory_t *);

// 
char *fetch(struct memory_t *);

// 
struct memory_t *push(struct memory_t *, const char *);

#endif