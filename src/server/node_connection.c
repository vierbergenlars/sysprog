#include <stddef.h>
#include "node_connection.h"

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
}

void node_connection_destroy(node_connection* conn)
{
    free(conn);
}
    
int node_connection_is_alive(node_connection* conn)
{
    return node->last_comm_time > time(NULL) - CONNECTION_TIMEOUT;
}

size_t node_connection_read(node_connection* conn)
{
    size_t bytes_to_read = sizeof(sensor_wire_data) - node->last_data_offset;
    size_t bytes_read = read(node->socket, (node->last_data+node->last_data_offset), bytes_to_read);
    if(bytes_read > 0) {
        node->last_data_offset+=bytes_read;
        node->last_communication = time(NULL);
    } else if(bytes_read == 0) {
        node->last_communication = 0;
    }
    return bytes_read;
}

sensor_wire_data* node_connection_read_buffer(node_connection* conn)
{
    if(node->last_data_offset < sizeof(sensor_wire_data))
        return NULL;
    sensor_wire_data* data = malloc(sizeof(sensor_wire_data));
    if(data == NULL)
        return NULL;
    memcpy(data, node->last_data, sizeof(sensor_wire_data));
    node->last_data = 0;
    return data;
}

node_connection* node_connection_find_by_socket(list_t* list, tcp_socket sock)
{
        int _find_sock(node_connection* conn)
        {
            return conn->fd = sock;
        }
        return list_find(list, _find_sock);
}

void node_connection_close_by_socket(list_t* list, tcp_socket sock)
{
        int _find_sock(node_connection* conn)
        {
            return conn->fd = sock;
        }
        int index = list_find_index(list, _find_sock);
        list_free_at_index(index);
}

int node_connection_add_socket(list_t* list, tcp_socket sock)
{
    node_connection* conn = node_connection_create(sock);
    if(conn == NULL)
        return 0;
    if(list_insert_at_index(list, conn, 0) == NULL)
        return 0;
    return 1;
}



