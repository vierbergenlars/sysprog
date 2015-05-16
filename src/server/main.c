#include <pthread.h>
#include <stdlib.h>
#include "log.h"
#include "../util/shared_queue.h"
#include "connection_manager.h"

int main(void)
{
    if(!log_start()) {
        fprintf(stderr, "Cannot open the log.\n");
        return EXIT_FAILURE;
    }
    pthread_t connection_manager;
    //pthread_t storage_manager;
    //pthread_t data_manager;

    queue* main_queue = queue_create(15, sizeof(sensor_wire_data));
    shared_queue* shared_queue = shared_queue_create(main_queue);

    int port = 1234;

    pthread_create(&connection_manager, NULL, connection_manager_th, connection_manager_configure(port, shared_queue));

    pthread_join(connection_manager, NULL);

    shared_queue_free(shared_queue);
    queue_free(main_queue);
}

