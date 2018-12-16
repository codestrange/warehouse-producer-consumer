#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "connections/connection.h"
#include "products/product.h"

Warehouse wh;

void* process_client(void *raw_data) {
    int clientfd = *((int*) raw_data); //Contiene el fd de la conexion del cliente
    //Falta parsear la peticion del cliente
    ProductList rp = new_productlist(10); //Solo para que compile
    bool writing = false; //Indica si el cliente es productor o consumidor
    for (int i = 0; i < rp.size; ++i) {
        bool finded = false;
        Product pc = index_productlist(&rp, i);
        for (int j = 0; j < wh.products.size; ++j) {
            Product pw = index_productlist(&wh.products, j);
            if (strlen(pw.name) == strlen(pc.name) && strcmp(pw.name, pc.name) == 0) {
                finded = true;
                for (int times = 0; times < pc.count; ++times) {
                    if (writing) {
                        dprintf(clientfd, "Comenze a colocar el producto %s en el almacen.\n", pc.name);
                        product_insert(&pw, &wh);
                        dprintf(clientfd, "Termine de colocar el producto %s en el almacen.\n", pc.name);
                    }
                    else {
                        dprintf(clientfd, "Comenze a consumir el producto %s en el almacen.\n", pc.name);
                        product_remove(&pw, &wh);
                        dprintf(clientfd, "Termine de consumir el producto %s en el almacen.\n", pc.name);
                    }
                }
            }
        }
        if (!finded) {
            printf("El producto no se encontraba.\n");
        }
    }

    return 0;
}

int main(int argc, char const *argv[]) {
    
    /*Preparando el servidor*/
    int port = atoi(argv[1]);
    int listenfd = get_listen_fd(port);
    /*Fin de la Preparación*/

    /*Iniciando la estructura del almacen*/
    int limit = atoi(argv[2]);
    warehouse_init(&wh, limit);
    if (limit == 0) {
        wh.products = parseProductList((char**)(argv + 2));
    }
    else {
        wh.products = parseProductList((char**)(argv + 3));
    }
    for (int i = 0; i < wh.products.size; ++i) {
        Product p = index_productlist(&(wh.products), i);
        remove_productlist(&(wh.products), i);
        product_init(&p, p.count);
        printf("name:%s count:%d limit:%d\n", p.name, p.count, p.limit);
        insert_productlist(&(wh.products), i, p);
    }
    /*Fin de la inicialización*/

    /*Ciclo Principal*/ 
    while(true){
        int clientfd = get_client_fd(listenfd);
        pthread_t ct;
        pthread_create(&ct, NULL, &process_client, &clientfd);
        pthread_detach(ct);
    }
    return 0;
}
