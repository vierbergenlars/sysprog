#include "connection_manager.h"
#include "log.h"
#include <stdlib.h>
#include "../util/tcp_socket.h"
#include "../util/list.h"
#include "../util/shared_queue.h"
#include "node_connection.h"

struct _connection_manager_info {
    int port;
    shared_queue* queue;
};


void* connection_manager_th(void* data)
{
    connection_manager_info *settings = data;

    tcp_socket server_sock = tcp_listen(settings->port);
    list_t* connection_list = node_connection_create_list();

    // on data callback
    int  _on_data(tcp_select* sel, tcp_socket sock)
    {
        LOG("New data packet on socket %d", sock);
        node_connection* conn = node_connection_find_by_socket(connection_list, sock);
        if(conn == NULL)
            return 0; // Close this connection
        // Close connection if no data was read, keep it open otherwise
        if(node_connection_read(conn) <= 0)
            return 0;
        sensor_wire_data* wire_data = node_connection_read_buffer(conn);
        if(wire_data != NULL) {
            LOG("Enqueued data packet from socket %d", sock);
            shared_queue_enqueue(settings->queue, wire_data);
        }
        return 1;
    }

    // on close callback
    int _on_close(tcp_select* sel, tcp_socket sock)
    {
        LOG("Closing connection on socket %d", sock);
        node_connection_remove_by_socket(connection_list, sock);
        return 0;
    }

    // on accept callback
    int _on_accept(tcp_select* sel, tcp_socket sock)
    {
        LOG("Accepted new connection. Socket %d", sock);
        if(!node_connection_add_socket(connection_list, sock))
            perror("node_connection_add_socket");
        return 1;
    }

    tcp_select* sel = tcp_select_create(server_sock, _on_accept, NULL, _on_data, _on_close);

    void _cleanup_sockets(tcp_socket sock)
    {
        node_connection* conn = node_connection_find_by_socket(connection_list, sock);
        if(conn != NULL) {
            if(!node_connection_is_alive(conn))
                tcp_select_close_socket(sel, sock);
        }
    }

    while(1) {
        tcp_select_wait(sel);
        tcp_select_foreach(sel, _cleanup_sockets);
    }

}

void* connection_manager_configure(int port, shared_queue* queue)
{
    connection_manager_info* config = malloc(sizeof(connection_manager_info));
    if(config == NULL)
        return NULL;
    config->port = port;
    config->queue = queue;
    return config;
}
