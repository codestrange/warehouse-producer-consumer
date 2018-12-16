#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "connections/connection.h"
#include "products/product.h"

Warehouse wh;

void* process_client(void *raw_data) {
    int clientfd = *((int*) raw_data);

    //Falta parsear la peticion del cliente
    ProductList request = new_productlist(10);//Solo para que compile
    bool writing = false;//Indica si el cliente es productor o consumidor

    return 0;
}

int main(int argc, char const *argv[]) {
    /*Prepare Warehouse Server*/
    int port = atoi(argv[1]);
    int listenfd = get_listen_fd(port);
    /*End Preparing Server*/
    /*Inicialize Warehouse Structure*/
    wh.limit = atoi(argv[2]);
    if (wh.limit == 0) {
        wh.products = parseProductList((char**)(argv + 2));
    }
    else {
        wh.products = parseProductList((char**)(argv + 3));
    }
    wh.limit = -1;
    for (int i = 0; i < wh.products.size; ++i) {
        Product p = index_productlist(&(wh.products), i);
        remove_productlist(&(wh.products), i);
        p.limit = p.count;
        p.used = malloc(1 * sizeof(sem_t));
        sem_init(p.used, 0, 1);
        p.count = 0;
        insert_productlist(&(wh.products), i, p);
    }
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
