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
    dprintf(clientfd, "Llegue al almacen.\n");// Notificar entrada
    for (int rp_index = 0; rp_index < rp.size; ++rp_index) {
        Product req = index_productlist(&rp, rp_index);
        dprintf(clientfd, "Buscando producto %s.\n", req.name);// Notificar busqueda
        bool end = false;
        for (int p_index = 0; p_index < wh.products.size; ++p_index) {//Buscar si el producto a usar está almacenado    
            Product act = index_productlist(&(wh.products), p_index);
            if (strlen(req.name) == strlen(act.name) && strcmp(req.name, act.name) == 0) {//Productos iguales?
                dprintf(clientfd, "Encontre el producto %s.\n", req.name);// Notificar hallazgo
                while (true) {
                    sem_wait(act.used);// Esperar hasta que no este utilizado el producto
                    if (writing) { //Productor
                        dprintf(clientfd, "Compruebando si puedo almacenar %d unidades del producto %s.\n", req.count, req.name);// Notificar intento
                        if (((wh.limit < 0) || (wh.act_cant + req.count <= wh.limit)) && ((act.limit < 0) || (req.count + act.count <= act.limit))) {
                            wh.act_cant += req.count;
                            act.count += req.count;
                            end = true;
                            //Actualizar producto en la lista
                            wh.products.array[p_index].count = act.count;
                        }
                        else {
                            dprintf(clientfd, "No pude almacenar %d unidades del producto %s. Esperando...\n", req.count, req.name);// Notificar intento
                        }
                    }
                    else { //Consumidor
                        dprintf(clientfd, "Compruebando si puedo consumir %d unidades del producto %s.\n", req.count, req.name);// Notificar intento
                        if (act.count - req.count >= 0) {
                            wh.act_cant -= req.count;
                            act.count -= req.count;
                            end = true;
                            //Actualizar producto en la lista
                            wh.products.array[p_index].count = act.count;
                        }
                        else {
                            dprintf(clientfd, "No pude consumir %d unidades del producto %s. Esperando...\n", req.count, req.name);// Notificar intento
                        }
                    }
                    sem_post(act.used);// Terminar de utilizar el producto
                    if (end) break; // Si se pudo completar la petición del producto continua sino espera
                }
                if (end) break;
            }
            // if (!end) {//Producto no encontrado
            //     --rp_index;//Forzar nueva revision
            //     //Añadir el nuevo producto al listado del almacen
            //     Product newp;
            //     newp.name = strdup(req.name);
            //     newp.count = 0;
            //     newp.limit = -1;// Nuevos productos no tienes limite en el almacen
            //     newp.used = malloc(1 * sizeof(sem_t));
            //     sem_init(newp.used, 0, 1);
            // }
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
    /*Fin de la inicialización*/

    /*Ciclo Principal*/ 
    while(true){
        int clientfd = get_client_fd(listenfd);
        pthread_t ct;
        pthread_create(&ct, NULL, &process_client, &clientfd);
    }
    return 0;
}
