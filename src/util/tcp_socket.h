#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_
#include <sys/select.h>
typedef int tcp_socket;
typedef struct _tcp_select tcp_select;
typedef int tcp_select_callback(tcp_select*, tcp_socket);
typedef void tcp_select_error_callback(tcp_select*, int);

tcp_socket tcp_listen(int port);
tcp_select* tcp_select_create(
        tcp_socket,
        tcp_select_callback,
        tcp_select_error_callback,
        tcp_select_callback,
        tcp_select_callback       
);
void tcp_select_wait(tcp_select*);
void tcp_select_foreach_fds(fd_set*, int, void (*)(tcp_socket));
void tcp_select_foreach(tcp_select*, void (*)(tcp_socket));
void tcp_select_destroy(tcp_select*);
void tcp_select_close_socket(tcp_select*, tcp_socket);
#endif
