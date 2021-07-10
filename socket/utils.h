#ifndef SOCKET_UTILS_H_
#define SOCKET_UTILS_H

#define ERORR_BUFFER_SIZE 1024
#define BUFFER_LEN 8200

#define TRUE 1
#define FALSE 0

#define EXIT_REQUEST_MESSAGE "exit"

#include <netinet/in.h>

// new errors trigger
extern int error_cond;

// errors are written in  this buffer
extern char error_message[ERORR_BUFFER_SIZE];

// errors interface
#define get_warning() (error_cond ? NULL : error_message + (error_cond = 0)) 
#define set_warning(...) ((error_cond = 1) ? sprintf(error_message, __VA_ARGS__) : 0)

#define fatal_error(...) {          \
    fprintf(stderr, __VA_ARGS__);   \
    exit(EXIT_FAILURE);             \
}

// creates a socket and returns the file descriptor
int create_socket();

// creates address object from ip and port
struct sockaddr_in create_address(short, const char *);

#endif