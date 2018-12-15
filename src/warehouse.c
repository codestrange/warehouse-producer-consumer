#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "connections/connection.h"
#include "products/product.h"

Warehouse wh;

int main(int argc, char const *argv[]) {
    /*Prepare Warehouse Server*/
    int port = atoi(argv[1]);
    int listenfd = get_listen_fd(port);
    /*End Preparing Server*/
    /*Inicialize Warehouse Structure*/
    wh.limit = atoi(argv[2]);
    wh.act_cant = 0;
    //Falta iniciar la lista de productos de wh
    /*End Inicialization*/

    /*Main Loop*/ 
    while(true){
        int clientfd = get_client_fd(listenfd);
        pthread_t ct;
        pthread_create(&ct, NULL, &process_client, &clientfd);
    }
    
    return 0;
}

void process_client(void *raw_data) {
    int clientfd = *((int*) raw_data);
    //Falta parsear la peticion del cliente
    return;
}