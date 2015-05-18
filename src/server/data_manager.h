#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_
#include "../util/queue.h"
#include <signal.h>
#include <stdio.h>
typedef struct _data_manager_info data_manager_info;
void* data_manager_th(void* data);
void* data_manager_configure(queue*, FILE* ,volatile sig_atomic_t*);
#endif
