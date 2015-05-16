#include "../util/logger.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <poll.h>

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

    struct pollfd master[1];

    master[0].fd = inSock;
    master[0].events = POLLIN;

    while(1) {
        if(poll(master, 1, -1) > 0) {
            if(master[0].revents == POLLHUP) {
                fflush(outFd);
                fclose(outFd);
                fclose(inFd);
                exit(EXIT_SUCCESS);
            }
            char line[1024];
            int skipBytes = 0;
            ssize_t readBytes;
read_piece:
            readBytes = read(inSock, line+skipBytes, 1024-skipBytes);
try_nextline:
            for(int i = skipBytes; i < readBytes; i++) {
                if(line[i] == '\n') {
                    line[i] = '\0';
                    fprintf(outFd, "%d %d %s\n", ++seq, (int)time(NULL), line);
                    fflush(outFd);
                    memmove(line, &line[i+1], 1024-i);
                    readBytes-=i;
                    skipBytes = 0;
                    goto try_nextline;
                } else if(line[i] == '\0') {
                    skipBytes = i;
                    goto read_piece;
                }
            }
        }
    }
    return EXIT_FAILURE;
}

