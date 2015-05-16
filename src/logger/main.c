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
    int inSock = fileno(inFd);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(inSock, &master);
    while(1) {
        fd_set fds = master;
        if(select(inSock+1,&fds, NULL, NULL, NULL) > 0) {
            char line[1024];
            ssize_t readBytes = read(inSock, line, 1024);
try_nextline:
            for(int i = 0; i < readBytes; i++) {
                if(line[i] == '\n') {
                    line[i] = '\0';
                    fprintf(outFd, "%d %d %s\n", ++seq, (int)time(NULL), line);
                    if(strncmp(line, "shutdown", 8) == 0)
                        return EXIT_SUCCESS;
                    memmove(line, &line[i+1], 1024-i);
                    readBytes-=i;
                    goto try_nextline;
                }
            }
        }
    }
    fclose(inFd);
    fclose(outFd);
    return EXIT_SUCCESS;
}

