#ifndef _CONFIG_H
#define _CONFIG_H

#define DB_HOST "localhost"
#define DB_USER "a13_syssoft"
#define DB_PASSWORD "a13_syssoft"
#define DB_DATABASE "a13_syssoft"
#define DB_TABLE "vierbergenlars"

#include <inttypes.h>
#include <time.h>
typedef struct __attribute__ ((__packed__)) {
    uint16_t sensor_id;
    double temp;
    time_t timestamp;
} sensor_wire_data;

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;

typedef struct {
    sensor_id_t sensor_id;
    int room_id;
    struct list* hist_data;
} sensor_data;
#endif
