#include "utils.h"

#include <sys/mman.h> // Shared memory functions
#include <fcntl.h> // O_* flags
#include <unistd.h> // ftruncate

#include <string.h>

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fatal_error("Usage: %s text\n", argv[0]);
    }

    char buffer[BUFFER_LEN] = {};
    strcpy(buffer, argv[1]);
    for(int i = 2; i < argc; ++i) {
        strcat(buffer, " ");
        strcat(buffer, argv[i]);
    }

    int req_len = strlen(buffer);

    int write_shm_fd = shm_open(SERVER_RESPONSE_SHARED_MEMORY_NAME, O_RDWR, 0777);

    if(write_shm_fd <= 0) {
        fatal_error("can not open shared memory %s", SERVER_RESPONSE_SHARED_MEMORY_NAME);
    }

    struct memory_t *response_memory = (struct memory_t *)mmap(NULL, sizeof(struct memory_t), PROT_WRITE | PROT_READ, MAP_SHARED, write_shm_fd, 0); // create a mapping to shared memory 

    int read_shm_fd = shm_open(SERVER_REQUEST_SHARED_MEMROY_NAME, O_RDWR, 0777);
    
    if(read_shm_fd <= 0) {
        fatal_error("can not open shared memory %s", SERVER_REQUEST_SHARED_MEMROY_NAME);
    }

    struct memory_t *request_memory = (struct memory_t *)mmap(NULL, sizeof(struct memory_t), PROT_WRITE | PROT_READ, MAP_SHARED, read_shm_fd, 0);

    clock_t start = clock();
    push(request_memory, buffer);
    char *response = fetch(response_memory);
    clock_t end = clock();

    int res_len = strlen(response);

    printf("server response: %s\n", response);
    printf("send: %d characters, receive: %d characters\n", req_len, res_len);
    printf("response time: %f mili-seconds\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);

    free(response);
    return 0;
}