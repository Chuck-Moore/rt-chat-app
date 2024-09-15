#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "list.h"
#include "inhandler.h"
#include "outhandler.h"
#include "dataTx.h"
#include "dataRx.h"
#include "shtdnmanager.h"

static int connect_socket(const char *port_local, const char *hostname_rem, const char *port_rem) {
    int sock = -1;
    struct addrinfo hints, *servinfo, *p;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL, port_local, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if(bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            continue;
        }
        break;
    }

    if(p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(servinfo);

    if((status = getaddrinfo(hostname_rem, port_rem, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if(connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }
    }

    if(p == NULL) {
        fprintf(stderr, "server: failed to connect\n");
        exit(2);
    }

    freeaddrinfo(servinfo);
    return sock;
}

int main(int argc, char *argv[]) {
    int sock;
    List *data_inc = List_create();
    List *data_out = List_create();

    if(data_inc == NULL || data_out == NULL) {
        fprintf(stderr,"Error creating the lists\n");
	    exit(1);
    }

    // two seperate lock for each list but it is not good with this list implementation
    // pthread_mutex_t data_out_lock = PTHREAD_MUTEX_INITIALIZER;
    // pthread_mutex_t data_inc_lock = PTHREAD_MUTEX_INITIALIZER;

    // Mutex used to solve the critical section problem when using lists
    // The two lists share the same mutex because they share the same pool of nodes (cs when creating/deleting new nodes)
    pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t data_out_signal = PTHREAD_COND_INITIALIZER;
    pthread_cond_t data_inc_signal = PTHREAD_COND_INITIALIZER;

    if(argc != 4) {
        fprintf(stderr,"usage: s-talk your_port remote_hostname remote_port\n");
	    exit(1);
    }

    char *port_local = argv[1];
    char *hostname_rem = argv[2];
    char *port_rem = argv[3];

    sock = connect_socket(port_local, hostname_rem, port_rem);

    // Initialize all threads
    Inhandler_init(data_out, &list_lock, &data_out_signal);
    DataTx_init(data_out, sock, &list_lock, &data_out_signal);
    DataRX_init(data_inc, sock, &list_lock, &data_inc_signal);
    Outhandler_init(data_inc, &list_lock, &data_inc_signal);

    waitShutdown();

    // Free the leftover dynamically allocated items in the lists, if any
    List_free(data_inc, free);
    List_free(data_out, free);

    close(sock);
    return 0;
}