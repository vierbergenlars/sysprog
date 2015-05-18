#include <stdio.h>
#include <malloc.h>
#include "sensor.h"
#include <time.h>
#include <assert.h>
#include "../util/queue.h"

/**
 * Creates a new sensor with a sensor id and a room id
 * @return sensor_data*|NULL
 */
sensor_data* sensor_create(int sensor_id, int room_id) {
    // Allocate memory for a sensor
    sensor_data* sensor = malloc(sizeof(sensor_data));
    if(sensor == NULL)
        return NULL;
    sensor->sensor_id = sensor_id;
    sensor->room_id = room_id;
    sensor->last_modified = 0;
    // Assign a historical data list
    sensor->hist_data = queue_create(5, sizeof(double));
    if(sensor->hist_data == NULL) {
        free(sensor);
        return NULL;
    }
    return sensor;
}

/**
 * Frees a sensor
 */
void sensor_free(sensor_data* sensor) {
    queue_free(sensor->hist_data);
    free(sensor);
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
    if(sensor->last_modified > wire_data->timestamp)
        return 0;
    queue_enqueue(sensor->hist_data, &wire_data->temp);
    return 1;
}

/**
 * Gets the last modified time from a sensor
 * @return time_t
 */
time_t sensor_last_modified(sensor_data* sensor) {
    return sensor->last_modified;
}

/**
 * Calculates the average temperature over n samples on a sensor.
 *
 * If there are less samples available than the requested number, 
 * the average over all samples is returned
 * @return double
 */
double sensor_average(sensor_data* sensor) {
    double avg = 0.0;
    queue* hist_data = sensor->hist_data;
    size_t size = queue_size(hist_data);

    // queue starts with the data array, so we can reinterpret the queue as an array
    sensor_wire_data* data = (sensor_wire_data*)hist_data;
    for(int i=0; i < size; i++) {
        avg+=data[i].temp/size;
    }
    return avg;
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
