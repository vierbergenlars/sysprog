#include "../util/logger.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/select.h>

void* read_until_newline(FILE*);
int main(void)
{
    FILE* inFd = logger_open("r");
    if(inFd == NULL) {
        perror("logger_open");
        return EXIT_FAILURE;
    }
    FILE* outFd = fopen("gateway.log", "a");
    if(outFd == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    int seq = 0;

    while(1) {
        char line[1024];
        if(fgets(line, 1024, inFd) !=NULL) {
            fprintf(outFd, "%d %d %s", ++seq, (int)time(NULL), line);
            if(strncmp(line, "shutdown", 8) == 0)
                break;
        } else {
            sleep(1);
        }
    }
    fclose(inFd);
    fclose(outFd);
    return EXIT_SUCCESS;
}

