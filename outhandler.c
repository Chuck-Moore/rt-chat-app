#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "outhandler.h"
#include "shtdnmanager.h"

static pthread_t thread;
static pthread_mutex_t *p_lock;
static pthread_cond_t *p_appendSingal;
static List *p_List;
static char *p_item;

void *Outhandler_thread(void *arg) {

    while(1) {

        pthread_mutex_lock(p_lock);
        while(p_List->size == 0) {
            pthread_cond_wait(p_appendSingal, p_lock);
        }

        p_item = List_trim(p_List);
        pthread_mutex_unlock(p_lock);

        fputs(">>> Remote: ", stdout);
        fputs(p_item, stdout);

        if(*p_item == '!' && *(p_item+1) == '\n') {
            shutdown_all();
            return NULL;
        }

        free(p_item);
        p_item = NULL;
    }

    return NULL;
}

void Outhandler_init(List *data_inc, pthread_mutex_t *data_inc_lock, pthread_cond_t *signal)
{
    p_List = data_inc;
    p_lock = data_inc_lock;
    p_appendSingal = signal;
    pthread_create(&thread, NULL, Outhandler_thread, NULL);
}

void Outhandler_shutdown()
{
    pthread_cancel(thread);
    pthread_join(thread, NULL);

    if(p_item) {
        free(p_item);
        p_item = NULL;
    }
}
