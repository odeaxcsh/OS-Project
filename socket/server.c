#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"

#include <semaphore.h>

#define MAX_THREADS_NUM 8


// semaphor to control number of active threads
sem_t threads_sem;

// threads info store in this objects
static struct thread_status_t
{
    int done;
    int socket_fd;
    struct sockaddr_in address;
} threads_status[MAX_THREADS_NUM] = { 0 };

int termination_requested = FALSE;

void *response_thread(void *arg)
{
    char buffer[BUFFER_LEN] = { 0 };
    char respose_buffer[BUFFER_LEN] = { 0 };

    static const char response_message[] = "%d characters recieved!";
    struct thread_status_t *my_info = (struct thread_status_t *) arg;

    memset(buffer, 0, BUFFER_LEN);

    // recieve message from client
    recv(my_info->socket_fd, buffer, BUFFER_LEN, 0);

    if(!strcmp(buffer, EXIT_REQUEST_MESSAGE)) {
        termination_requested = TRUE;
    }

    int res_len = strlen(buffer);

    sprintf(respose_buffer, response_message, res_len);

    // send response message
    send(my_info->socket_fd, respose_buffer, strlen(respose_buffer) + 1, 0);
    
    // find client ip andport 
	char source[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(my_info->address.sin_addr), source, INET_ADDRSTRLEN);
    printf("from [%s:%d]: %s\n", source, ntohs(my_info->address.sin_port), buffer);

    // close connection
    close(my_info->socket_fd);

    // set thread status as done
    my_info->done = TRUE;

    // send signal to semaphor to say there is a new avaiable thread
    sem_post(&threads_sem);

    return NULL;
}

void thread_manager(int socket_fd, struct sockaddr_in address)
{
    // threads ip stored in this object
    static pthread_t threads_id[MAX_THREADS_NUM] = { 0 };

    // special case socket_fd <= 0 means termination occurred
    if(socket_fd <= 0) {
        for(int i = 0; i < MAX_THREADS_NUM; ++i) {
            if(threads_id[i]) {
                pthread_join(threads_id[i], NULL);
            }
        }

        return;
    }

    // wait for a free thread
    sem_wait(&threads_sem);

    // find a thread which is finished
    for(int i = 0; i < MAX_THREADS_NUM; ++i) {
        if(threads_status[i].done == TRUE) {

            // destroy previous thread
            if(threads_id[i] != 0)
                pthread_join(threads_id[i], NULL);

            // set new attributes
            threads_status[i].done = FALSE;
            threads_status[i].address = address;
            threads_status[i].socket_fd = socket_fd;
            
            // create new thread
            pthread_create(threads_id + i, NULL, response_thread, (void*)(threads_status + i));

            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int max_message_num = -1;

    // set default port and ip
    short port = 8080;
    char *ip = "127.0.0.1";

    if(sem_init(&threads_sem, 0, MAX_THREADS_NUM)) {
        fatal_error("initializing semaphore failed!");
    }

    // parsing command line arguments
    int c;
    while((c = getopt(argc, argv, "h:p:n:")) != -1) {
        switch(c) {
            case 'h':
                ip = optarg;
                break;

            case 'p':
                port = (int)strtol(optarg, NULL, 10);
                break;

            case 'n':
                max_message_num = (int)strtol(optarg, NULL, 10);
                break;

            default:
                fatal_error("Usage: %s [-h hostaddr] [-p portnumber] [-n response count limit] \n", argv[0]);
        }
    }

    // create socket and address object
    int socket_fd = create_socket();
    struct sockaddr_in address = create_address(port, ip);

    // bind socket with address
    if(bind(socket_fd, (struct sockaddr *)(&address), sizeof(address)) < 0) {
        fatal_error("failed to bind\n");
    }

    // start listening
    if(listen(socket_fd, -1) < 0) {
        fatal_error("failed to listen\n");
    }

    // initializating thread info objects
    for(int i = 0; i < MAX_THREADS_NUM; ++i) {
        threads_status[i].done = TRUE;
    }

    fprintf(stdout, "server is ready...\n");

    // number of responsed requests
    int count = 0;

    // server loop
    while(count != max_message_num && !termination_requested) {
        ++count;
        // connect to client
        struct sockaddr_in client_address;
        int client_addrlen, client_socket = accept(socket_fd, (struct sockaddr *)&client_address, (socklen_t*)&client_addrlen);

        if(client_socket <= 0) {
            close(socket_fd);
            fatal_error("failed to connet to client");
        }

        // create a new thread to response
        thread_manager(client_socket, client_address);
    }

    thread_manager(-1, address);

    close(socket_fd);

    return 0;
}