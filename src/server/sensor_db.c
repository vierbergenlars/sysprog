#define _GNU_SOURCE
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#define db_query_f(conn, sql) \
if(mysql_query(conn, sql)) { \
    fprintf(stderr, "%s\n", mysql_error(conn));\
    mysql_close(conn);\
    return NULL;\
}
#define db_query_r(conn, sql) \
if(mysql_query(conn, sql)) { \
    fprintf(stderr, "%s\n", mysql_error(conn)); \
    return NULL; \
} else { \
    return mysql_store_result(conn);\
}
#define db_query_p(conn, sql, args...) \
char proto[] = sql; \
char *query;\
if(asprintf(&query, proto, args) < 0)\
    return NULL;\
if(mysql_query(conn, query)) { \
    fprintf(stderr, "%s\n", mysql_error(conn)); \
    free(query);\
    return NULL; \
} else { \
    free(query);\
    return mysql_store_result(conn);\
}

/*
 * Make a connection to MySQL database
 * Create a table named 'yourname' if the table does not exist
 * If the table existed, clear up the existing data if clear_up_flag is set to 1
 * return the connection for success, NULL if an error occurs
 */
MYSQL *init_connection(char clear_up_flag) {
    MYSQL* conn = mysql_init(NULL);
    if(conn == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    if(!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_DATABASE, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }
    if(clear_up_flag) {
        db_query_f(conn, "DROP TABLE IF EXISTS `"DB_TABLE"`;");
    }
    db_query_f(conn, "CREATE TABLE IF NOT EXISTS `"DB_TABLE"` ( \
        `id` int(11) NOT NULL AUTO_INCREMENT, \
        `sensor_id` int(11) NOT NULL,\
        `sensor_value` decimal(4,2) NOT NULL,\
        `timestamp` timestamp NOT NULL,\
        PRIMARY KEY (`id`),\
        KEY `sensor_value` (`sensor_value`),\
        KEY `timestamp` (`timestamp`)\
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=1;");

    return conn;
}

/*
 * Disconnect MySQL database
 */
void disconnect(MYSQL *conn) {
    mysql_close(conn);
}

/*
 * Write an INSERT query to insert a single sensor measurement
 * return zero for success, and non-zero if an error occurs
 */

int insert_sensor(MYSQL *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    char query_proto[] = "INSERT INTO `"DB_TABLE"` (`sensor_id`, `sensor_value`, `timestamp`) VALUES(%d, %f, FROM_UNIXTIME(%d));";
    char* query;
    if(asprintf(&query, query_proto, id, value, ts) < 0)
        return -1;

    int ret = mysql_query(conn, query);
    free(query);
    return ret;
}

/*
 * Write a SELECT query to return all sensor measurements existed in the table
 * return MYSQL_RES with all the results
 */

MYSQL_RES *find_sensor_all(MYSQL *conn) {
    db_query_r(conn, "SELECT * FROM`"DB_TABLE"`;");
}

/*
 * Write a SELECT query to return all sensor measurements containing 'value_t'
 * return MYSQL_RES with all the results
 */

MYSQL_RES *find_sensor_by_value(MYSQL *conn, sensor_value_t value) {
    db_query_p(conn, "SELECT * FROM `"DB_TABLE"` WHERE `sensor_value` = %f", value);
}

/*
 * Write a SELECT query to return all sensor measurement that its value exceeds 'value_t'
 * return MYSQL_RES with all the results
 */
MYSQL_RES *find_sensor_exceed_value(MYSQL *conn, sensor_value_t value) {
    db_query_p(conn, "SELECT * FROM `"DB_TABLE"` WHERE `sensor_value` > %f", value);
}

/*
 * Write a SELECT query to return all sensor measurement containing timestamp 'ts_t'
 * return MYSQL_RES with all the results
 */
MYSQL_RES *find_sensor_by_timestamp(MYSQL *conn, sensor_ts_t ts) {
    db_query_p(conn, "SELECT * FROM `"DB_TABLE"` WHERE UNIX_TIMESTAMP(`timestamp`) = %d", ts);
}

/*
 * Write a SELECT query to return all sensor measurement recorded later than timestamp 'ts_t'
 * return MYSQL_RES with all the results
 */
MYSQL_RES *find_sensor_later_timestamp(MYSQL *conn, sensor_ts_t ts) {
    db_query_p(conn, "SELECT * FROM `"DB_TABLE"` WHERE UNIX_TIMESTAMP(`timestamp`) > %d", ts);
}

/*
 * Return the number of records contained in the result
 */
int get_result_size(MYSQL_RES *result) {
    return mysql_num_rows(result);
}

/*
 * Print all the records contained in the result
 */
void print_result(MYSQL_RES *result) {
    MYSQL_ROW row;
    
    while((row = mysql_fetch_row(result)) != NULL) {
        printf("%s %s %s\n", row[1], row[2], row[3]);
    }

}
