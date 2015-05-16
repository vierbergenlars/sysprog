#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>
extern FILE* log_file;
#define LOG(args...) fprintf(log_file, args)
void log_stop(void);
int log_start(void);
#endif
