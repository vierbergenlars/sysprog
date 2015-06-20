#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>
extern FILE* log_file;
#ifdef DEBUG
#define DEBUG_LOG(fmt, args...) LOG(fmt, ##args)
#else
#define DEBUG_LOG(fmt, args...) do {}while(0)
#endif

#define LOG(fmt, args...) do { \
    fprintf(log_file, "%s: " fmt"\n",__FILE__, ##args); \
    fflush(log_file); \
} while(0)
void log_stop(void);
int log_start(void);
#endif
