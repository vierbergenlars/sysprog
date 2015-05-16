#include "_tcpsocket.h"
#include "tcp_socket.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>

tcp_socket tcp_listen(int port)
{
    Socket sock = tcp_passive_open(port);
    if(sock == NULL)
        return -1;
    int fd = get_socket_descriptor(sock);
    int fd2 = dup(fd); 
    tcp_close(&sock);
    return fd2;
}

struct _tcp_select {
    fd_set fds;
    int max_fd;
    int server_fd;
    tcp_select_callback *on_accept;
    tcp_select_error_callback *on_accept_error;
    tcp_select_callback *on_data;
    tcp_select_callback *on_close;
};

int tcp_select_callback_empty(tcp_select* t, tcp_socket s)
{
    return 1;
}

void tcp_select_error_callback_empty(tcp_select* t, tcp_socket s)
{
}
#define CB_SELECT_SET(s, cb) s->cb = (((cb) == NULL)?tcp_select_callback_empty:(cb))
#define CB_SEL_ERR_SET(s, cb) s->cb = (((cb) == NULL)?tcp_select_error_callback_empty:(cb))

tcp_select* tcp_select_create(
        tcp_socket                server_fd,
        tcp_select_callback       on_accept,
        tcp_select_error_callback on_accept_error,
        tcp_select_callback       on_data,
        tcp_select_callback       on_close
)
{
    tcp_select* sel = malloc(sizeof(tcp_select));
    if(sel == NULL)
        return NULL;
    sel->server_fd = server_fd;
    FD_ZERO(&sel->fds);
    sel->max_fd = sel->server_fd;
    FD_SET(sel->server_fd, &sel->fds);
    CB_SELECT_SET(sel, on_accept);
    CB_SEL_ERR_SET(sel, on_accept_error);
    CB_SELECT_SET(sel, on_data);
    CB_SELECT_SET(sel, on_close);
    return sel;
}

void tcp_select_destroy(tcp_select* sel)
{
    void _close_all(tcp_socket i) {
        close(i);
    }
    tcp_select_foreach_fds(&sel->fds, sel->max_fd, _close_all);
    close(sel->server_fd);
    free(sel);
}

void tcp_select_wait(tcp_select* sel)
{
    void _handle_select(tcp_socket i) {
        if(i == sel->server_fd) {
            int new_client = accept(i, NULL, NULL);
            if(new_client < 0) {
                sel->on_accept_error(sel, errno);
            } else {
                if(sel->on_accept(sel, new_client)) {
                    FD_SET(new_client, &sel->fds);
                    if(sel->max_fd < new_client)
                        sel->max_fd = new_client;
                }
            }
        } else {
            if(!sel->on_data(sel, i)) {
                tcp_select_close_socket(sel, i);
            }
        }
    }
    fd_set fds = sel->fds;
    if(select(sel->max_fd+1, &fds, NULL, NULL, NULL) > 0) {
        tcp_select_foreach_fds(&fds, sel->max_fd, _handle_select);
    }
}

void tcp_select_foreach_fds(fd_set *fds, int fd_max, void (*cb)(tcp_socket))
{
    for(int i = 0; i <= fd_max; i++)
        if(FD_ISSET(i, fds)) {
            cb(i);
        }
}

void tcp_select_foreach(tcp_select *sel, void(*cb)(tcp_socket))
{
    tcp_select_foreach_fds(&sel->fds, sel->max_fd, cb);
}

void tcp_select_close_socket(tcp_select *sel, tcp_socket i)
{
    if(FD_ISSET(i, &sel->fds)) {
        FD_CLR(i, &sel->fds);
        sel->on_close(sel, i);
        close(i);
    }
}

