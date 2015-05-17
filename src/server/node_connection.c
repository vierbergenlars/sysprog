#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "node_connection.h"
#include "config.h"

#define CONNECTION_TIMEOUT 15
struct _node_connection {
    tcp_socket fd;
    time_t last_comm_time;
    char last_data[sizeof(sensor_wire_data)];
    char last_data_offset;
};

node_connection* node_connection_create(tcp_socket fd)
{
    node_connection* conn = malloc(sizeof(node_connection));
    if(conn == NULL)
        return NULL;
    conn->fd = fd;
    conn->last_comm_time = time(NULL);
    memset(conn->last_data, 0, sizeof(sensor_wire_data));
    conn->last_data_offset = 0;
    return conn;
}

void node_connection_destroy(node_connection* conn)
{
    free(conn);
}
    
int node_connection_is_alive(node_connection* conn)
{
    return conn->last_comm_time > time(NULL) - CONNECTION_TIMEOUT;
}

size_t node_connection_read(node_connection* conn)
{
    size_t bytes_to_read = sizeof(sensor_wire_data) - conn->last_data_offset;
    size_t bytes_read = read(conn->fd, (conn->last_data+conn->last_data_offset), bytes_to_read);
    if(bytes_read > 0) {
        conn->last_data_offset+=bytes_read;
        conn->last_comm_time = time(NULL);
    } else if(bytes_read == 0) {
        conn->last_comm_time = 0;
    }
    return bytes_read;
}

sensor_wire_data* node_connection_read_buffer(node_connection* conn)
{
    if(conn->last_data_offset < sizeof(sensor_wire_data))
        return NULL;
    sensor_wire_data* data = malloc(sizeof(sensor_wire_data));
    if(data == NULL)
        return NULL;
    memcpy(data, conn->last_data, sizeof(sensor_wire_data));
    conn->last_data_offset = 0;
    return data;
}

node_connection* node_connection_find_by_socket(list_t* list, tcp_socket sock)
{
        int _find_sock(void* conn)
        {
            return ((node_connection*)conn)->fd = sock;
        }
        return list_find(list, _find_sock);
}

void node_connection_remove_by_socket(list_t* list, tcp_socket sock)
{
        int _find_sock(void* conn)
        {
            return ((node_connection*)conn)->fd = sock;
        }
        int index = list_find_index(list, _find_sock);
        list_free_at_index(list, index);
}

int node_connection_add_socket(list_t* list, tcp_socket sock)
{
    node_connection* conn = node_connection_create(sock);
    if(conn == NULL)
        return 0;
    if(list_insert_at_index(list, conn, 0) == NULL) {
        free(conn);
        return 0;
    }
    free(conn);
    return 1;
}

void _node_connection_copy(void** dest, void* src)
{
    *dest = malloc(sizeof(node_connection));
    if(*dest != NULL)
        memcpy(*dest, src, sizeof(node_connection));
}

void _node_connection_free(void** el)
{
    free(*el);
    *el = NULL;
}

list_t* node_connection_create_list()
{
    return list_create(_node_connection_copy, _node_connection_free, NULL, NULL);
}
