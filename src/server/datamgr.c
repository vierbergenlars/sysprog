#include <stdio.h>
#include <stdlib.h>
#include "../util/list.h"
#include <inttypes.h>
#include "datamgr.h"
#include "sensor.h"

void _sensor_copy(void** d, void* s) {
    *d = s;
}

void _sensor_free(void** o) {
    sensor_free(*o);
}
/**
 * Creates a new sensor list
 * @return list_t*|NULL
 */
list_t* sensor_list_create() {

    return list_create(&_sensor_copy, &_sensor_free, NULL, NULL);
}

/**
 * Creates a new sensor list and adds sensors from a mapping file
 * @return list_t*|NULL
 */
list_t* sensor_list_from_file(FILE* mapping_f) {
    list_t* sensors = sensor_list_create();
    if(sensors == NULL)
        return NULL;

    while(!feof(mapping_f)) {
        int sensor_id, room_id;
        // Load one sensor - room mapping from the file
        if(fscanf(mapping_f, "%d %d\n", &room_id, &sensor_id) < 2) {
            perror("fscanf");
            list_free(&sensors);
            return NULL;
        }

        // Create a new sensor for the mapping
        sensor_data* sensor = sensor_create(sensor_id, room_id);
        if(sensor == NULL) {
            list_free(&sensors);
            return NULL;
        }

        // Add sensor to the list
        list_t* result = list_insert_at_index(sensors, sensor, 0);
        if(result == NULL||list_errno != 0) {
            list_free(&sensors);
            return NULL;
        }
    }

    return sensors;
}

/**
 * Locates the sensor that the received packet is from
 * @return sensor_data*|NULL
 */
sensor_data* sensor_list_find_sensor(list_t* sensors, sensor_wire_data* packet) {
    int sensor_equals(void* sensor) {
        return ((sensor_data*)sensor)->sensor_id == packet->sensor_id;
    }
    return list_find(sensors, &sensor_equals);
}

