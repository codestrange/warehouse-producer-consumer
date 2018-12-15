#ifndef PRODUCT
#define PRODUCT
#include <semaphore.h>

typedef struct ProductWarehouse {
    char *name;
    int cant;
    int limit;
    sem_t *used;
} ProductWarehouse;

typedef struct Warehouse {
    int act_cant;
    int limit;
} Warehouse;

#endif