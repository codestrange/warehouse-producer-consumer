#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "connections/connection.h"
#include "products/product.h"

Warehouse wh;

void print_status(Warehouse *wh) {
    sem_wait(wh->mutex);
    printf("Estado del Almacen\nAlmacenados: %d Capacidad: %d\n", wh->act_cant, wh->limit);
    for (int i = 0; i < wh->products.size; ++i) {
        Product p = index_productlist(&wh->products, i);
        printf("Producto %s\nAlmacenados: %d\nCapacidad: %d\n",p.name, p.count, p.limit);
    }
    sem_post(wh->mutex);
}

void* process_client(void *raw_data) {
    char *buf = malloc(2050 * sizeof(char));
    bzero(buf, 2050);
    int clientfd = *((int*) raw_data); //Contiene el fd de la conexion del cliente
    free(raw_data);
    read(clientfd, buf, 2048);
    // printf("Procesando peticion: %s.\n", buf);
    Request req = parseRequest(buf);
    ProductDataList rp = req.products;
    bool writing = false;
    if (req.type == PRODUCER)
        writing = true;
    else if (req.products.size == 0) {
        // printf("ANY\n");
        while(true) {
            for (int j = 0; j < get_size(&wh); ++j) {
                Product *pw = indexref_productlist(&wh.products, j);
                ProductData ret;
                product_tryremove(pw, &wh, &ret);
                if (ret.id < 0) {
                    continue;
                }
                dprintf(clientfd,"%s %s %d %s\n", ret.name, ret.producer_name, ret.id, ret.data);    
            }
        }
    }
    for (int i = 0; i < rp.size; ++i) {
        bool finded = false;
        ProductData pc = index_productdatalist(&rp, i);
        for (int j = 0; j < get_size(&wh); ++j) {
            Product *pw = indexref_productlist(&wh.products, j);
            if (strlen(pw->name) == strlen(pc.name) && strcmp(pw->name, pc.name) == 0) {
                finded = true;
                if (writing) {
                    // printf("Almacenando producto de tipo %s.\n", pc.name);
                    product_insert(pw, &wh, pc);
                    // printf("Producto de tipo %s almacenado.\n", pc.name);
                }
                else {
                    ProductData ret;
                    // printf("Consumiendo producto de tipo %s.\n", pc.name);
                    product_remove(pw, &wh, &ret);
                    // printf("Consumido producto de tipo %s.\n", pc.name);
                    printf("Devuelto %s %s %d %s\n", ret.name, ret.producer_name, ret.id, ret.data);
                    dprintf(clientfd,"%s %s %d %s\n", ret.name, ret.producer_name, ret.id, ret.data);
                    //free_product_data(&ret);
                }
            }
        }
        if (!finded) {
            printf("El producto no se encontraba.\n");
            sem_wait(wh.mutex);
            Product newp;
            product_init(&newp, 0);
            newp.productsData = new_productdatalist(100);
            newp.name = pc.name;
            append_productlist(&wh.products, newp);
            sem_post(wh.mutex);
            --i;
        }
    }
    printf("Fin de peticion.\n");
    close(clientfd);
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
        p.productsData = new_productdatalist(100);
        insert_productlist(&(wh.products), i, p);
    }
    /*Fin de la inicialización*/

    /*Ciclo Principal*/ 
    while (true) {
        printf("Esperando Conexión.\n");
        int *clientfd = malloc(1 * sizeof(int*)); 
        *clientfd = get_client_fd(listenfd);
        pthread_t *ct = malloc(1 * sizeof(pthread_t));
        printf("Conexión establecida.\n");
        print_status(&wh);
        pthread_create(ct, NULL, &process_client, clientfd);
        pthread_detach(*ct);
        free(ct);
    }
    return 0;
}
