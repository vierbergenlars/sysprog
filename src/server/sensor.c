#include <stdio.h>
#include <malloc.h>
#include "sensor.h"
#include <time.h>
#include <assert.h>

/**
 * Creates a new sensor with a sensor id and a room id
 * @return sensor_data*|NULL
 */
sensor_data* sensor_create(int sensor_id, int room_id) {
    /**
     * Copies sensor wire data
     */
    void _histdata_copy(void** d, void* s) {
        *d = malloc(sizeof(sensor_wire_data));
        sensor_wire_data** dest = (sensor_wire_data**)d;
        sensor_wire_data* src = (sensor_wire_data*)s;
        (*dest)->sensor_id = src->sensor_id;
        (*dest)->temp = src->temp;
        (*dest)->timestamp = src->timestamp;
    }
    /**
     * Frees sensor wire data
     */
    void _histdata_free(void** e) {
        free(*e);
        *e = NULL;
    }
    /**
     * Prints sensor wire data
     */
    void _histdata_print(void* e) {
        sensor_wire_data *el = (sensor_wire_data*)e;
        printf("sensor_data_histdata { Sensor_id %d; Temp: %f; Timestamp: %d }\n", el->sensor_id, el->temp, (int)el->timestamp);
    }

    // Allocate memory for a sensor
    sensor_data* sensor = malloc(sizeof(sensor_data));
    if(sensor == NULL)
        return NULL;
    sensor->sensor_id = sensor_id;
    sensor->room_id = room_id;
    // Assign a historical data list
    sensor->hist_data = list_create(&_histdata_copy, &_histdata_free, NULL, &_histdata_print);
    if(sensor->hist_data == NULL) {
        free(sensor);
        return NULL;
    }
    return sensor;
}

/**
 * Internal sensor free function that does not null the memory location
 * @internal
 */
void __sensor_free(sensor_data* sensor) {
    list_free(&sensor->hist_data);
    free(sensor);
}

/**
 * Prints the data of a sensor
 */
void sensor_print(sensor_data* sensor) {
    printf("sensor_data { id: %d, room: %d, avg5: %f, tot: %d, upd: %d\n", sensor->sensor_id, sensor->room_id, sensor_average(sensor, 5), list_size(sensor->hist_data), (int)sensor_last_modified(sensor));
    list_print(sensor->hist_data);
    printf("}\n");
}

/**
 * Creates a full copy of a sensor
 * @return sensor_data*|NULL
 */
sensor_data* sensor_clone(sensor_data* src) {
    // First create a new sensor
    sensor_data* dest = sensor_create(src->sensor_id, src->room_id);
    if(dest == NULL)
        return NULL;
    // Then copy all historical data
    if(!list_copy(src->hist_data, dest->hist_data)) {
        sensor_free(dest);
        return NULL;
    }
    return dest;
}

/**
 * Updates sensor data with a data packet
 *
 * The sensor id of *sensor and *wire_data must match.
 * If the last modified time of *sensor is after the timestamp on *wire_data, the measurement is discarded.
 * @return 0 on failure, 1 on success
 */
int sensor_update(sensor_data* sensor, sensor_wire_data* wire_data) {
    assert(sensor->sensor_id == wire_data->sensor_id);
    if(sensor_last_modified(sensor) > wire_data->timestamp)
        return 0;
    return list_insert_at_index(sensor->hist_data, wire_data, 0) != NULL;
}

/**
 * Gets the last modified time from a sensor
 * @return time_t
 */
time_t sensor_last_modified(sensor_data* sensor) {
    sensor_wire_data* wire_data = (sensor_wire_data*)list_get_element_at_index(sensor->hist_data, 0);
    if(wire_data == NULL)
        return 0;
    return wire_data->timestamp;
}

/**
 * Calculates the average temperature over n samples on a sensor.
 *
 * If there are less samples available than the requested number, 
 * the average over all samples is returned
 * @return double
 */
double sensor_average(sensor_data* sensor, int n) {
    double avg = 0.0;
    list_t* hist_data = sensor->hist_data;
    int i;
    for(i=0; i < n; i++) {
        sensor_wire_data* wire_data = (sensor_wire_data*)list_get_element_at_index(hist_data, i);
        if(wire_data == NULL)
            break;
        avg+=wire_data->temp;
    }
    return avg/i;
}

/**
 * Gets the sensor id
 * @return int
 */
int sensor_id(sensor_data* sensor) {
    return sensor->sensor_id;
}

/**
 * Gets the sensor room id
 * @return int
 */
int sensor_room(sensor_data* sensor) {
    return sensor->room_id;
}
