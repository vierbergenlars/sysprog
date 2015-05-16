#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

FILE* logger_open(const char* mode)
{
    if(mkfifo("logFifo", S_IRUSR|S_IWUSR) < 0) {
        if(errno != EEXIST) {
            return NULL;
        }
    }
    return fopen("logFifo", mode);
}
