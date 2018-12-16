#ifndef PRODUCT
#define PRODUCT
#include <semaphore.h>
#include "../utils/list.h"

typedef struct Product {
    char *name;
    char *prodid;
    int count;
    int limit;
    sem_t *mutex;
    sem_t *slots;
    sem_t *items;
} Product;

typedef struct ProductData {
    char *name;
    char *producer_name;
    int id;
    char* data;
} ProductData;

typedef struct ProductList {
    Product *array;
    int size;
    int capacity;
} ProductList;

typedef struct Warehouse {
    int act_cant;
    int limit;
    ProductList products;
    sem_t *mutex;
    sem_t *slots;
    sem_t *items;
} Warehouse;

ProductList new_productlist(int capacity);

void insert_productlist(ProductList *list, int index, Product item);

void append_productlist(ProductList *list, Product item);

void clear_productlist(ProductList *list);

Product remove_productlist(ProductList *list, int index);

Product pop_productlist(ProductList *list);

Product index_productlist(ProductList *list, int index);

void free_productlist(ProductList *list);

void free_product(Product *product);

int parseInt(CharList *charList);

ProductList parseProductList(char **arg);

void product_init(Product *p, int limit);

void warehouse_init(Warehouse *wh, int limit);

void product_insert(Product *p, Warehouse *wh);

void product_remove(Product *p, Warehouse *wh);

#endif