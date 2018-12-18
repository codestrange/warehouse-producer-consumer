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
    if (argc < 2) {
        printf("Error: Se necesitan un parametro. La dirección del almacén.\n");
        return 0;
    }
    ProductList products = new_productlist(10);
    ProductList servers = new_productlist(10);
    parseArguments(argv + 1, &products, &servers);
    if (!servers.size) {
        printf("Error: Se necesitan un parametro. La dirección del almacén.\n");
        return 0;
    }
    int index_server = -1;
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
                    printf("Consumiendo del Almacén con IP: %s y Puerto: %d el producto con Nombre: %s y Cantidad: %d\n", server.name, server.count, actual.name, actual.count);
                    dprintf(clientfd, "consumer %s:%d", actual.name, actual.count);
                    char c;
                    while (read(clientfd, &c, 1)) {
                        write(1, c, 1);
                    }
                    printf("Consumo terminado del Almacén con IP: %s y Puerto: %d\n", server.name, server.count);
                } else {
                    printf("Error conectandose con el Almacén con IP: %s y Puerto: %d\n", server.name, server.count);
                }
            }
        }
        if (index_server + 1 >= servers.size)
            index_server = -1;
    }
}
