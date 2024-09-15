#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dataRx.h"

#define MAXBUFLEN 1024

int sock;
static pthread_t thread;
static pthread_mutex_t *p_lock;
static pthread_cond_t *p_appendSignal;
List *p_list;

void *DataRX_thread(void *arg) {
    char buf[MAXBUFLEN];

    while(1) {
        int numbytes;

        if((numbytes = recv(sock, buf, MAXBUFLEN, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        if(numbytes == 0) {
            printf("remote closed the connection\n");
        } else {
            char *item = malloc(numbytes);
            strcpy(item, buf);
            pthread_mutex_lock(p_lock);
            if(List_prepend(p_list, item) == -1) {
                fprintf(stderr, "List_prepend error");
                exit(1);
            }
            pthread_cond_signal(p_appendSignal);
            pthread_mutex_unlock(p_lock);

            if(buf[0] == '!' && buf[1] == '\n') {
                return NULL;
            }
        }
    }

    return NULL;
}

void DataRX_init(List *data_inc, int socket, pthread_mutex_t *data_inc_lock, pthread_cond_t *signal)
{
    sock = socket;
    p_lock = data_inc_lock;
    p_appendSignal = signal;
    p_list = data_inc;
    pthread_create(&thread, NULL, DataRX_thread, NULL);
}

void DataRx_shutdown()
{
    pthread_cancel(thread);
    pthread_join(thread, NULL);
}
