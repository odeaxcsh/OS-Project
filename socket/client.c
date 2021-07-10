#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

#include "utils.h"
  
int main(int argc, char **argv)
{
    // set default port and ip
    short port = 8080;
    char *ip = "127.0.0.1";

    // parsing command line arguments
    int c;
    while((c = getopt(argc, argv, "h:p:")) != -1) {
        switch(c) {
            case 'h':
                ip = optarg;
                break;

            case 'p':
                port = (int)strtol(optarg, (char **)NULL, 10);
                break;

            default:
                fatal_error("Usage: %s [-h hostaddr] [-p portnumber] texts ...\n", argv[0]);
        }
    }

    if(optind >= argc) {
        fatal_error("Usage: %s [-h hostaddr] [-p portnumber] texts ...\n", argv[0]);
    }

    // concat extra arguments
    char buffer[BUFFER_LEN] = {};
    strcpy(buffer, argv[optind++]);
    for(; optind < argc; ++optind) {
        strcat(buffer, " ");
        strcat(buffer, argv[optind]);
    }

    int req_len = strlen(buffer);

    // create socket and address object
    int socket_fd = create_socket();
    struct sockaddr_in address = create_address(port, ip);

    // connect the client socket to the server
    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) != 0) {
        fatal_error("connection with the server failed...\n");
    }    

    clock_t start = clock();

    // send request message
    send(socket_fd, buffer, strlen(buffer) + 1, 0);
    
    // recieve message from server
    recv(socket_fd, buffer, BUFFER_LEN, 0);

    clock_t end = clock();

    int res_len = strlen(buffer);

    printf("server response: %s\n", buffer);
    printf("send: %d characters, receive: %d characters\n", req_len, res_len);
    printf("response time: %f mili-seconds\n", (double)(end - start) / CLOCKS_PER_SEC * 1000);

    // close the socket
    close(socket_fd);

    return 0;
}