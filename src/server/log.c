#include "../util/logger.h"
FILE* log_file = NULL;

int log_start(void) {
    if(log_file == NULL)
        log_file = logger_open("w");
    if(log_file == NULL||ferror(log_file)) {
        perror("logger_open");
        return 0;
    }
    return 1;
}

void log_stop(void) {
    if(log_file != NULL)
        fclose(log_file);
}
