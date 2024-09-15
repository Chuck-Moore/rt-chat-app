#include <pthread.h>

#include "shtdnmanager.h"
#include "dataRx.h"
#include "dataTx.h"
#include "inhandler.h"
#include "outhandler.h"

static pthread_cond_t syncOkToShtdn = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int waiting = 1;

void waitShutdown()
{
    pthread_mutex_lock(&lock);
    while(waiting) {
        pthread_cond_wait(&syncOkToShtdn, &lock);
    }
    pthread_mutex_unlock(&lock);

    DataRx_shutdown();
    Outhandler_shutdown();
    Inhandler_shutdown();
    DataTx_shutdown();
    printf("Program terminating.\n");
}

void shutdown_all()
{
    pthread_mutex_lock(&lock);
    waiting = 0;
    pthread_cond_signal(&syncOkToShtdn);
    pthread_mutex_unlock(&lock);
}
