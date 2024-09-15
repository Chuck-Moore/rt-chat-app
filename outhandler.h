#ifndef _OUTHANDLER_H_
#define _OUTHANDLER_H_

#include <pthread.h>

#include "list.h"

void Outhandler_init(List *data_inc, pthread_mutex_t *data_inc_lock, pthread_cond_t *signal);

void Outhandler_shutdown();

#endif