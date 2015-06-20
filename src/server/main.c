#include <pthread.h>
#include <stdlib.h>
#include "log.h"
#include "../util/shared_queue.h"
#include "connection_manager.h"
#include "storage_manager.h"
#include "data_manager.h"
#include "sensor.h"
#include <signal.h>

#define PTHREAD_CREATE(pthread, fn, data) \
    if(pthread_create(&pthread, NULL, fn, data)) { \
        fprintf(stderr, "Cannot create thread "#pthread": %m\n");\
        return EXIT_FAILURE; \
    }
#define PTHREAD_JOIN(pthread) \
    if(pthread_join(pthread, NULL)) { \
        fprintf(stderr, "Cannot join thread "#pthread": %m\n"); \
        return EXIT_FAILURE; \
    }

int main(int argc, char** argv)
{

    volatile sig_atomic_t shutdown_flag = 0;
    int port = 1234;
    pthread_t connection_manager;
    pthread_t storage_manager;
    pthread_t data_manager;

    if(argc > 1)
        port = atoi(argv[1]);

    if(!log_start()) {
        fprintf(stderr, "Cannot open the log.\n");
        return EXIT_FAILURE;
    }

    void _signal_handler(int sig)
    {
        LOG("Signaled, shutting down\n");
        fprintf(stderr, "Shutting down operations\n");
        shutdown_flag = 1;
    }
    signal(SIGINT, _signal_handler);
    signal(SIGHUP, _signal_handler);
    signal(SIGTERM, _signal_handler);

    queue* main_queue = queue_create(15, sizeof(sensor_wire_data));
    if(main_queue == NULL) {
        fprintf(stderr, "Cannot create main queue: %m\n");
        return EXIT_FAILURE;
    }

    void _main_queue_on_overflow()
    {
        LOG("Queue is full: overwriting data");
    }
    queue_on_overflow(main_queue, &_main_queue_on_overflow);
    shared_queue* shared_queue = shared_queue_create(main_queue);
    if(shared_queue == NULL) {
        fprintf(stderr, "Cannot create shared queue: %m\n");
        return EXIT_FAILURE;
    }

    FILE* mapping_file = fopen("mapping_file", "r");
    if(mapping_file == NULL) {
        fprintf(stderr, "Cannot open mapping_file: %m\n");
        return EXIT_FAILURE;
    }

    PTHREAD_CREATE(connection_manager, connection_manager_th, connection_manager_configure(port, shared_queue, &shutdown_flag));

    queue* storage_manager_queue = shared_queue_fork(shared_queue);
    if(storage_manager_queue == NULL) {
        fprintf(stderr, "Cannot create storage manager queue: %m\n");
        return EXIT_FAILURE;
    }

    PTHREAD_CREATE(storage_manager, storage_manager_th, storage_manager_configure(storage_manager_queue, &shutdown_flag));
    PTHREAD_CREATE(data_manager, data_manager_th, data_manager_configure(main_queue, mapping_file, &shutdown_flag));

    PTHREAD_JOIN(connection_manager);
    PTHREAD_JOIN(storage_manager);
    PTHREAD_JOIN(data_manager);
    
    queue_unfork(storage_manager_queue);
    shared_queue_free(shared_queue);
    queue_free(main_queue);
    fclose(mapping_file);
    log_stop();
    return EXIT_SUCCESS;
}

