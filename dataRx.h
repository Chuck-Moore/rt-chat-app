#ifndef _DATARX_H_
#define _DATARX_H_

#include <pthread.h>

#include "list.h"

void DataRX_init(List *data_inc, int socket, pthread_mutex_t *data_inc_lock, pthread_cond_t *signal);

void DataRx_shutdown();

#endif