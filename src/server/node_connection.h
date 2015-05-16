#ifndef _NODE_CONNECTION_H_
#define _NODE_CONNECTION_H_
#include <time.h>
#include "sensor.h"
#include "../util/tcp_socket.h"
#include "../util/list.h"
typedef struct _node_connection node_connection;

/**
 * Creates a new node connection from a socket
 * @return node_connection*|NULL
 */
node_connection* node_connection_create(tcp_socket fd);

/**
 * Destroys a node connection
 * This does not close the socket associated with the connection
 */
void node_connection_destroy(node_connection* conn);

/**
 * Checks if the node connection is still to be considered alive
 * @return int 0 if the connection is dead, 1 if it is alive
 */
int node_connection_is_alive(node_connection* conn);

/**
 * Attempts to read one data-packet from the connection.
 * If a full packet could not be read, the partially read result is cached,
 * and subsequent reads try to complete the packet.
 * @return int The number of bytes read this time
 */
size_t node_connection_read(node_connection* conn);

/**
 * Read out the buffer stored from reading a packet.
 * If there is no full packet available, NULL is returned
 * @return sensor_wire_data*|NULL
 */
sensor_wire_data* node_connection_read_buffer(node_connection* conn);

/**
 * Locates a node connection in the list by its tcp socket
 * @return node_connection*|NULL
 */
node_connection* node_connection_find_by_socket(list_t* list, tcp_socket sock);

/**
 * Removes a node connection in the list by its tcp socket
 * This does not close the socket associated with the connection.
 */
void node_connection_remove_by_socket(list_t* list, tcp_socket sock);

/**
 * Creates a node connection for a tcp socket and adds it to the list.
 * @return int 0 if it failed, 1 if successful
 */
int node_connection_add_socket(list_t* list, tcp_socket sock);
#endif
