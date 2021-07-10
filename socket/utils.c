#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char error_message[ERORR_BUFFER_SIZE];

int create_socket()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd < 0) {
        fatal_error("socket creation failed");
    }

    return socket_fd;
}


struct sockaddr_in create_address(short port, const char *ip)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);

    if(address.sin_addr.s_addr <= 0) {
        fatal_error("%s is not a valid IPv4", ip);
    }

    return address;
}
