#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inhandler.h"

#define MAXBUFLEN 1024

static List *list;
static pthread_t thread;
static pthread_mutex_t *p_lock;
static pthread_cond_t *p_prependSignal;

void *Inhandler_thread(void *arg)
{
    char buf[MAXBUFLEN];
    buf[MAXBUFLEN-1] = '\0'; // don't really need this, extra protection
 
    while(1) {
        size_t p = 0;
        while(1) {
            if(read(STDIN_FILENO, &buf[p], 1) <= 0) {
                perror("read");
                exit(1);
            }
            if(buf[p] == '\n' || p >= MAXBUFLEN-2){
                buf[p+1] = '\0';
                break;
            }
            p++;
        }

        char *str = malloc(strlen(buf)+1);
        if(str == NULL) {
            perror("malloc");
            exit(1);
        }
        strcpy(str, buf);

        pthread_mutex_lock(p_lock);
        if(List_prepend(list, str) == -1) {
            fprintf(stderr, "List_prepend error");
            exit(1);
        }
        pthread_cond_signal(p_prependSignal);
        pthread_mutex_unlock(p_lock);

        if(*buf == '!' && *(buf+1) == '\n') {
            return NULL;
        }
    }

    return NULL;
}

void Inhandler_init(List *data_out, pthread_mutex_t *data_out_lock, pthread_cond_t *signal)
{
    list = data_out;
    p_lock = data_out_lock;
    p_prependSignal = signal;
    pthread_create(&thread, NULL, Inhandler_thread, NULL);
}

void Inhandler_shutdown()
{
    pthread_cancel(thread);
    pthread_join(thread, NULL);
}
