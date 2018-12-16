#include <stdlib.h>
#include "product.h"
#include "../utils/list.h"
#include "../utils/parser.h"

ProductList new_productlist(int capacity) {
    ProductList list;
    list.capacity = capacity;
    list.array = malloc(capacity * sizeof(Product));
    list.size = 0;
    return list;
}

void insert_productlist(ProductList *list, int index, Product item) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->array = realloc(list->array, list->capacity * sizeof(Product));
    }
    for (int i = list->size; i > index; --i)
        list->array[i] = list->array[i - 1];
    list->array[index] = item;
    ++list->size;
}

void append_productlist(ProductList *list, Product item) {
    insert_productlist(list, list->size, item);
}

void clear_productlist(ProductList *list) {
    list->size = 0;
}

Product remove_productlist(ProductList *list, int index) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    Product item = list->array[index];
    for (int i = index; i < list->size - 1; ++i)
        list->array[i] = list->array[i + 1];
    --list->size;
    return item;
}

Product pop_productlist(ProductList *list) {
    return remove_productlist(list, list->size - 1);
}

Product index_productlist(ProductList *list, int index) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    return list->array[index];
}

void free_productlist(ProductList *list) {
    for (int i = 0; i < list->size; ++i) {
        Product product = index_productlist(list, i);
        free_product(&product);
    }
    free(list->array);
}

void free_product(Product *product) {
    free(product->name);
}

int parseInt(CharList *charList) {
    int integer = 0;
    for (int i = 0; i < charList->size; ++i) {
        char c = index_charlist(charList, i);
        integer = integer * 10 + (c - '0');
    }
    return integer;
}

ProductList parseProductList(char **arg) {
    ProductList productList = new_productlist(10);
    CharCharList str = convert_CharCharList(arg);
    for (int i = 0; i < str.size; ++i) {
        CharList temp = index_charcharlist(&str, i);
        char *chartemp = convert_arraychar(&temp);
        char **splited = str_split(chartemp, ':');
        free(chartemp);
        CharCharList charCharList = convert_CharCharList(splited);
        char **t = splited;
        while (*t) {
            free(*t);
            ++t;
        }
        free(splited);
        if (charCharList.size >= 2) {
            Product product;
            CharList charListName = index_charcharlist(&charCharList, 0);
            CharList charListCount = index_charcharlist(&charCharList, 1);
            product.name = convert_arraychar(&charListName);
            product.count = parseInt(&charListCount);
            append_productlist(&productList, product);
        }
        free_charcharlist(&charCharList);
    }
    free_charcharlist(&str);
    return productList;
}

void product_init(Product *p, int limit) {
    if (limit == 0) limit = RAND_MAX;
    p->limit = limit;
    p->count = 0;
    p->mutex = malloc(1 * sizeof(sem_t));
    p->slots = malloc(1 * sizeof(sem_t));
    p->items = malloc(1 * sizeof(sem_t));
    sem_init(p->mutex, 0, 1);
    sem_init(p->slots, 0, limit);
    sem_init(p->items, 0, 0);
}

void warehouse_init(Warehouse *wh, int limit) {
    if (limit == 0) limit = RAND_MAX;
    wh->limit = limit;
    wh->act_cant = 0;
    wh->mutex = malloc(1 * sizeof(sem_t));
    wh->slots = malloc(1 * sizeof(sem_t));
    wh->items = malloc(1 * sizeof(sem_t));
    sem_init(wh->mutex, 0, 1);
    sem_init(wh->slots, 0, limit);
    sem_init(wh->items, 0, 0);
}

void product_insert(Product *p, Warehouse *wh, ProductData data) {
    sem_wait(p->slots);
    sem_wait(wh->slots);
    sem_wait(p->mutex);
    sem_wait(wh->mutex);
    ++p->count;
    ++wh->act_cant;
    append_productdatalist(&p->productsData, data);
    sem_post(wh->mutex);
    sem_post(p->mutex);
    sem_wait(wh->items);
    sem_post(p->items);
}

void product_remove(Product *p, Warehouse *wh, ProductData *data) {
    sem_wait(p->items);
    sem_wait(wh->items);
    sem_wait(p->mutex);
    sem_wait(wh->mutex);
    --p->count;
    --wh->act_cant;
    ProductData ret = remove_productdatalist(&p->productsData, 0);
    data->name = ret.name;
    data->id = ret.id;
    data->producer_name = ret.producer_name;
    data->data = ret.data;
    sem_post(wh->mutex);
    sem_post(p->mutex);
    sem_post(wh->slots);
    sem_post(p->slots);
}

ProductDataList new_productdatalist(int capacity) {
    ProductDataList list;
    list.capacity = capacity;
    list.array = malloc(capacity * sizeof(ProductData));
    list.size = 0;
    return list;
}

void insert_productdatalist(ProductDataList *list, int index, ProductData item) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->array = realloc(list->array, list->capacity * sizeof(ProductData));
    }
    for (int i = list->size; i > index; --i)
        list->array[i] = list->array[i - 1];
    list->array[index] = item;
    ++list->size;
}

void append_productdatalist(ProductDataList *list, ProductData item) {
    insert_productdatalist(list, list->size, item);
}

void clear_productdatalist(ProductDataList *list) {
    list->size = 0;
}

ProductData remove_productdatalist(ProductDataList *list, int index) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    ProductData item = list->array[index];
    for (int i = index; i < list->size - 1; ++i)
        list->array[i] = list->array[i + 1];
    --list->size;
    return item;
}

ProductData pop_productdatalist(ProductDataList *list) {
    return remove_productdatalist(list, list->size - 1);
}

ProductData index_productdatalist(ProductDataList *list, int index) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    return list->array[index];
}

void free_productdatalist(ProductDataList *list) {
    for (int i = 0; i < list->size; ++i) {
        ProductData item = index_productdatalist(list, i);
        free_product_data(&item);
    }
    free(list->array);
}

void free_product_data(ProductData *item) {
    free(item->name);
    free(item->data);
    free(item->producer_name);
}