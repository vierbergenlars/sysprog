#ifndef _STORAGE_MANAGER_H_
#define _STORAGE_MANAGER_H_
#include "../util/queue.h"
#include <signal.h>
typedef struct _storage_manager_info storage_manager_info;
void* storage_manager_th(void* data);
void* storage_manager_configure(queue*, volatile sig_atomic_t*);
#endif
