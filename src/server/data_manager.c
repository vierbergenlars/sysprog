#include "data_manager.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../util/queue.h"
#include "../util/list.h"
#include "datamgr.h"
#include "log.h"

#ifndef SET_MIN_TEMP
#error SET_MIN_TEMP has to be defined at compile-time
#endif
#ifndef SET_MAX_TEMP
#error SET_MAX_TEMP has to be defined at compile-time
#endif

struct _data_manager_info {
    queue* q;
    FILE* mapping_file;
    volatile sig_atomic_t* shutdown_flag;
};

void* data_manager_th(void* data)
{
    data_manager_info* settings = data;
    list_t* nodes = sensor_list_from_file(settings->mapping_file);

    while(1) {
        if(queue_size(settings->q) == 0) {
            if(*settings->shutdown_flag)
                break;
            sleep(1);
            continue;
        }

        sensor_wire_data* data = queue_dequeue(settings->q);
        if(data == NULL) {
            LOG("Read a null data packet?!");
            continue;
        }
        sensor_data* sensor = sensor_list_find_sensor(nodes, data);
        if(sensor == NULL) {
            LOG("Unmapped sensor (id=%d); ignored", data->sensor_id);
            continue;
        }
        if(!sensor_update(sensor, data)) {
            LOG("Stale data from sensor (id=%d); ignored", data->sensor_id);
            continue;
        }

        if(queue_size(sensor->hist_data) == 5) {
            double avg = sensor_average(sensor);
            if(avg < SET_MIN_TEMP)
                LOG("Room %d is too cold (sensor %d)", sensor_room(sensor), sensor_id(sensor));
            else if(avg > SET_MAX_TEMP)
                LOG("Room %d is too hot (sensor %d)", sensor_room(sensor), sensor_id(sensor));
        }
    }

    LOG("Shutting down");

    list_free(&nodes);
    free(data);
    return NULL;
}

void* data_manager_configure(queue* q, FILE* mapping_file, volatile sig_atomic_t* shutdown_flag)
{
    data_manager_info* settings = malloc(sizeof(data_manager_info));
    if(settings == NULL)
        return NULL;
    settings->q = q;
    settings->mapping_file = mapping_file;
    settings->shutdown_flag = shutdown_flag;
    return settings;
}
