#include <sys/mman.h> // Shared memory functions
#include <fcntl.h> // O_* flags
#include <unistd.h> // ftruncate

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#include "utils.h"

sem_t termination_signal;
int terminations_signal_value = 0;

// returns shared memory file descriptor
int create_shared_mm(const char *name, int size)
{
    /**
    * O_RDWR: Open the object for read-write access
    * O_CRET: If object doesn't exist, create it
    * O_EXCL: If object already exists, throw an error
    */
    int shm_fd = shm_open(name, O_RDWR | O_CREAT, 0777);

    if(shm_fd <= 0) {
        fatal_error("creating %s failed", name);
    }

    // Set memory size
    ftruncate(shm_fd, size);
    
    return shm_fd;
}

void *response_thread(void *args)
{
    static const char *standard_response = "%d characters received";
    char response_buffer[BUFFER_LEN] = { 0 };

    tuple(struct memory_t *) *memories = args;
    struct memory_t * response_memory = memories->first;
    struct memory_t * request_memory = memories->second;

    while(!terminations_signal_value) {
        char *request = fetch(request_memory);
        
        if(terminations_signal_value) {
            continue;
        }

        fprintf(stdout, "received message: %s\n", request);

        if(!strcmp(request, EXIT_REQUEST_MESSAGE)) {
            terminations_signal_value = 1;
            sem_post(&termination_signal);
        }

        sprintf(response_buffer, standard_response, strlen(request));
        push(response_memory, response_buffer);
        free(request);
    }

    return NULL;
}

int main()
{
    if(sem_init(&termination_signal, 0, 0)) {
        fatal_error("error during creating semaphore");
    }

    int write_shm_fd = create_shared_mm(SERVER_RESPONSE_SHARED_MEMORY_NAME, sizeof(struct memory_t));
    struct memory_t *response_memory = (struct memory_t *)mmap(NULL, sizeof(struct memory_t), PROT_WRITE | PROT_READ, MAP_SHARED, write_shm_fd, 0); // create a mapping to shared memory 
    initialize(response_memory);

    int read_shm_fd = create_shared_mm(SERVER_REQUEST_SHARED_MEMROY_NAME, sizeof(struct memory_t));
    struct memory_t *request_memory = (struct memory_t *)mmap(NULL, sizeof(struct memory_t), PROT_WRITE | PROT_READ, MAP_SHARED, read_shm_fd, 0);
    initialize(request_memory);

    tuple(struct memory_t *) memories_tuple = {.first = response_memory, .second = request_memory};

    pthread_t thread_ids[MAX_NUMBER_OF_CLIENTS] = { 0 };

    for(int i = 0; i < MAX_NUMBER_OF_CLIENTS; ++i) {
        pthread_create(thread_ids + i, NULL, response_thread, &memories_tuple);
    }

    // wait till termination signal arrives
    sem_wait(&termination_signal);

    for(int i = 0; i < MAX_NUMBER_OF_CLIENTS; ++i) {
        sem_post(RELATIVE_ACCESS(request_memory, items));
    }

    for(int i = 0; i < MAX_NUMBER_OF_CLIENTS; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    // free shared memories space
    shm_unlink(SERVER_RESPONSE_SHARED_MEMORY_NAME);
    shm_unlink(SERVER_REQUEST_SHARED_MEMROY_NAME);

    return 0;
}