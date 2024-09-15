#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#include "dataTx.h"
#include "shtdnmanager.h"

static List *list;
static pthread_t thread;
static pthread_mutex_t *p_lock;
static pthread_cond_t *p_appendSignal; // used to signal a new item added to the list
static int sock; // socked FD used in UDP communication
static char *p_item;

void *DataTx_thread(void *arg) {

    while(1) {
        pthread_mutex_lock(p_lock);
        while(list->size == 0) {
            pthread_cond_wait(p_appendSignal, p_lock);
        }

        p_item = List_trim(list);
        pthread_mutex_unlock(p_lock);

        send(sock, p_item, strlen(p_item)+1, 0);

        if(*p_item == '!' && *(p_item+1) == '\n') {
            shutdown_all();
            return NULL;
        }

        free(p_item);
        p_item = NULL;
    }

    return NULL;
}

void DataTx_init(List *data_out, int socket, pthread_mutex_t *data_out_lock, pthread_cond_t *signal)
{
    list = data_out;
    sock = socket;
    p_lock = data_out_lock;
    p_appendSignal = signal;
    pthread_create(&thread, NULL, DataTx_thread, NULL);
}

void DataTx_shutdown()
{
    pthread_cancel(thread);
    pthread_join(thread, NULL);

    if(p_item) {
        free(p_item);
        p_item = NULL;
    }
}
