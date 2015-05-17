#include <pthread.h>
#include <stdlib.h>
#include "log.h"
#include "../util/shared_queue.h"
#include "connection_manager.h"
#include "config.h"
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

int main(void)
{
    if(!log_start()) {
        fprintf(stderr, "Cannot open the log.\n");
        return EXIT_FAILURE;
    }
    volatile sig_atomic_t shutdown_flag = 0;
    pthread_t connection_manager;
    //pthread_t storage_manager;
    //pthread_t data_manager;

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
    shared_queue* shared_queue = shared_queue_create(main_queue);
    if(shared_queue == NULL) {
        fprintf(stderr, "Cannot create shared queue: %m\n");
        return EXIT_FAILURE;
    }

    int port = 1234;

    PTHREAD_CREATE(connection_manager, connection_manager_th, connection_manager_configure(port, shared_queue, &shutdown_flag));


    PTHREAD_JOIN(connection_manager);
    shared_queue_free(shared_queue);
    queue_free(main_queue);
    log_stop();
    return EXIT_SUCCESS;
}

