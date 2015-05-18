#ifndef _SENSOR_H
#define _SENSOR_H
#include <stdint.h>
#include <time.h>
#include "../util/queue.h"
#include <time.h>

typedef struct {
    int sensor_id;
    int room_id;
    time_t last_modified;
    queue* hist_data;
} sensor_data;

typedef struct __attribute__ ((__packed__)) {
    uint16_t sensor_id;
    double temp;
    time_t timestamp;
} sensor_wire_data;

/**
 * Creates a new sensor with a sensor id and a room id
 * @return sensor_data*|NULL
 */
sensor_data* sensor_create(int, int);

/**
 * Destroys a sensor
 */
void sensor_free(sensor_data*);

/**
 * Prints the data of a sensor
 */
void sensor_print(sensor_data*);

/**
 * Creates a full copy of a sensor
 * @return sensor_data*|NULL
 */
sensor_data* sensor_clone(sensor_data*);

/**
 * Updates sensor data with a data packet
 *
 * The sensor id of *sensor and *wire_data must match.
 * If the last modified time of *sensor is after the timestamp on *wire_data, the measurement is discarded.
 * @return 0 on failure, 1 on success
 */
int sensor_update(sensor_data*, sensor_wire_data*);

// Getters
/**
 * Gets the last modified time from a sensor
 * @return time_t
 */
time_t sensor_last_modified(sensor_data*);

/**
 * Calculates the average temperature over 5 samples on a sensor.
 *
 * @return double
 */
double sensor_average(sensor_data*);

/**
 * Gets the sensor id
 * @return int
 */
int sensor_id(sensor_data*);

/**
 * Gets the sensor room id
 * @return int
 */
int sensor_room(sensor_data*);
#endif
