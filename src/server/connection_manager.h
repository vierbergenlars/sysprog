#ifndef _CONNECTION_MANAGER_H_
#define _CONNECTION_MANAGER_H_
#include "../util/tcp_socket.h"
#include "../util/shared_queue.h"
#include <inttypes.h>
typedef struct __attribute__ ((__packed__)) {
    uint16_t sensor_id;
    double temp;
    time_t timestamp;
} sensor_wire_data;
typedef struct _connection_manager_info connection_manager_info;
void* connection_manager_th(void* data);
void* connection_manager_configure(int, shared_queue*);
#endif
