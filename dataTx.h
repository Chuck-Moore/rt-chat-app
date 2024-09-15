#ifndef _DATATX_H_
#define _DATATX_H_

#include <pthread.h>

#include "list.h"

void DataTx_init(List *data_out, int socket, pthread_mutex_t *data_out_lock, pthread_cond_t *signal);

void DataTx_shutdown();

#endif