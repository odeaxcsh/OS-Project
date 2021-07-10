#include <string.h>

#include "utils.h"

char *fetch(struct memory_t *memory)
{
    // wait for a new object to be written on the memory
    sem_wait(RELATIVE_ACCESS(memory, items));

    // lock memroy before changing list_head
    pthread_mutex_lock(&(memory->fetch_lock));

    // fetch an item
    char *item = strdup(memory->memory[memory->list_head]);
    memory->list_head = (memory->list_head + 1) % memory->size;

    // unlock memory
    pthread_mutex_unlock(&(memory->fetch_lock));

    // increase number of empty slots of memory
    sem_post(RELATIVE_ACCESS(memory, empty));

    return item;
}

struct memory_t *push(struct memory_t *memory, const char *item)
{
    // wait to find a new empty slot
    sem_wait(RELATIVE_ACCESS(memory, empty));

    // lock memory before changing list_tail and other stuff
    pthread_mutex_lock(&(memory->push_lock));

    // copy item object and reset tail index
    strcpy(memory->memory[memory->list_tail], item);
    memory->list_tail = (memory->list_tail + 1) % memory->size;

    // unlcok memroy
    pthread_mutex_unlock(&(memory->push_lock));

    // increase number of items
    sem_post(RELATIVE_ACCESS(memory, items));

    // souls' desire
    return memory;
}

void initialize(struct memory_t *memory)
{
    memset(memory, 0, sizeof(struct memory_t));

    CONST_CAST(int, memory->size) = MAX_NUMBER_OF_CLIENTS;
    memory->list_head = memory->list_tail = 0;

    if(pthread_mutex_init(RELATIVE_ACCESS(memory, fetch_lock), NULL)) {
        fatal_error("can not initialize fetch mutext");
    }

    if(pthread_mutex_init(RELATIVE_ACCESS(memory, push_lock), NULL)) {
        fatal_error("can not initialize fetch mutext");
    }

    if(sem_init(RELATIVE_ACCESS(memory, empty), 1, memory->size)) {
        fatal_error("can not initialize `free` semaphore")
    }
    
    if(sem_init(RELATIVE_ACCESS(memory, items), 1, 0)) {
        fatal_error("can not initialize `items` semaphore")
    }
}