#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "products/product.h"
#define _POSIX_C_SOURCE 200809L

void parseArguments(char **args, ProductList *products, ProductList *servers) {
    ProductList arguments = parseProductList(args);
    for (int i = 0; i < arguments.size; ++i) {
        Product actual = index_productlist(&arguments, i);
        if (index(actual.name, '.')) {
            append_productlist(servers, actual);
        } else if (!strncmp(actual.name, "localhost", 9)) {
            free(actual.name);
            actual.name = "127.0.0.1";
            append_productlist(servers, actual);
        } else {
            append_productlist(products, actual);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Error: Se necesitan tres parametros. El nombre del productor, la dirección del almacén y el descripción de un producto.\n");
        return 0;
    }
    char *producer_name = argv[1];
    ProductList products = new_productlist(10);
    ProductList servers = new_productlist(10);
    parseArguments(argv + 2, &products, &servers);
    if (!servers.size || !products.size) {
        printf("Error: Se necesitan tres parametros. El nombre del productor, la dirección del almacén y el descripción de un producto.\n");
        return 0;
    }
    int index_server = -1;
    int counter_product = 0;
    while (true) {
        sleep(1);
        Product server = index_productlist(&servers, ++index_server);
        char *ip = server.name;
        int port = server.count;
        for (int i = 0; i < products.size; ++i) {
            Product actual = index_productlist(&products, i);
            int count = actual.count;
            while (count--) {
                int clientfd = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in sock;
                sock.sin_family = AF_INET;
                sock.sin_port = htons((short int)port);
                inet_aton(ip, &sock.sin_addr);
                if (!connect(clientfd, (struct sockaddr *)&sock, sizeof(sock))) {
                    printf("Produciendo hacia el Almacén con IP: %s y Puerto: %d el producto con Nombre: %s y Id: %d\n", server.name, server.count, actual.name, counter_product + 1);
                    dprintf(clientfd, "producer %s %s %d data", actual.name, producer_name, ++counter_product);
                    char c;
                    while (read(clientfd, &c, 1)) { }
                    printf("Producción terminada en el Almacén con IP: %s y Puerto: %d\n", server.name, server.count);
                } else {
                    printf("Error conectandose con el Almacén con IP: %s y Puerto: %d\n", server.name, server.count);
                }
                close(clientfd);
            }
        }
        if (index_server + 1 >= servers.size)
            index_server = -1;
    }
}
