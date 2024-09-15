#ifndef _INHANDLER_H_
#define _INHANDLER_H_

#include <pthread.h>

#include "list.h"

void Inhandler_init(List *data_out, pthread_mutex_t *data_out_lock, pthread_cond_t *signal);

void Inhandler_shutdown();

#endif