#include "../util/queue.h"
#include "log.h"
#include "storage_manager.h"
#include <stdlib.h>
#include <unistd.h>
#include "sensor_db.h"
#include "config.h"
#include "sensor.h"
#include <signal.h>

struct _storage_manager_info {
    queue* q;
    volatile sig_atomic_t* shutdown_flag;
};

void* storage_manager_th(void* data)
{
    storage_manager_info* settings = data;
    void* mysql_conn = NULL;
    int reinit_db = 1;
    goto connect;
retry_connection:
    sleep(10);
connect:
    if(*settings->shutdown_flag)
        goto shutdown;
    if(mysql_conn != NULL)
        disconnect(mysql_conn);
    mysql_conn = init_connection(reinit_db);
    if(mysql_conn == NULL) {
        LOG("Could not connect to mysql server");
        goto retry_connection;
    }
    reinit_db = 0;
    LOG("Connection to mysql server established");

    while(1) {
        if(queue_size(settings->q) == 0) {
            if(*settings->shutdown_flag)
                break;
            sleep(1);
            continue;
        }
        sensor_wire_data* data = queue_top(settings->q);
        if(data == NULL) {
            LOG("Got a NULL data packet?!");
            continue;
        }
        LOG("Dequeued packet (%d, %f, %d)", data->sensor_id, data->temp, data->timestamp);
        if(insert_sensor(mysql_conn, data->sensor_id, data->temp, data->timestamp)) {
            LOG("Connection to mysql server lost");
            free(data);
            goto connect;
        }
        free(data);
        free(queue_dequeue(settings->q));
    }
shutdown:
    LOG("Shutting down");
    if(mysql_conn != NULL)
        disconnect(mysql_conn);
    mysql_library_end();
    free(data);
    return NULL;
}

void* storage_manager_configure(queue* q, volatile sig_atomic_t* shutdown_flag)
{
    storage_manager_info* settings = malloc(sizeof(storage_manager_info));
    if(settings == NULL)
        return NULL;
    settings->q = q;
    settings->shutdown_flag = shutdown_flag;
    return settings;
}
