#include <stdlib.h>
#include <string.h>
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

Product *indexref_productlist(ProductList *list, int index) {
    if (index < 0)
        index = 0;
    if (index > list->size)
        index = list->size;
    return &list->array[index];
}

void free_product(Product *product) {
    free(product->name);
}

void free_productlist(ProductList *list) {
    for (int i = 0; i < list->size; ++i) {
        Product product = index_productlist(list, i);
        free_product(&product);
    }
    free(list->array);
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

void free_product_data(ProductData *item) {
    free(item->name);
    free(item->data);
    free(item->producer_name);
}
void free_productdatalist(ProductDataList *list) {
    for (int i = 0; i < list->size; ++i) {
        ProductData item = index_productdatalist(list, i);
        free_product_data(&item);
    }
    free(list->array);
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
    sem_post(wh->items);
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

void product_tryremove(Product *p, Warehouse *wh, ProductData *data) {
    if(sem_trywait(p->items) < 0) {
        data = NULL;
        return;
    }
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

int parseInt(CharList *charList) {
    int integer = 0;
    for (int i = 0; i < charList->size; ++i) {
        char c = index_charlist(charList, i);
        integer = integer * 10 + (c - '0');
    }
    return integer;
}

ProductList parseProductList(char **arg) {
    CharCharList str = convert_CharCharList(arg);
    ProductList productList = new_productlist(10);
    if (!(*arg))
        return productList;
    for (int i = 0; i < str.size; ++i) {
        CharList temp = index_charcharlist(&str, i);
        char *chartemp = convert_arraychar(&temp);
        char **splited = str_split(chartemp, ':');
        free(chartemp);
        CharCharList charCharList = convert_CharCharList(splited);
        while (**splited) {
            free(*splited);
            ++(*splited);
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

ProductDataList parseProductDataListConsumer(char **arg) {
    CharCharList str = convert_CharCharList(arg);
    ProductDataList products = new_productdatalist(10);
    if (!(*arg)) {
        free_charcharlist(&str);
        return products;
    }
    for (int i = 0; i < str.size; ++i) {
        CharList temp = index_charcharlist(&str, i);
        char *chartemp = convert_arraychar(&temp);
        char **splited = str_split(chartemp, ':');
        free(chartemp);
        CharCharList charCharList = convert_CharCharList(splited);
        while (**splited) {
            free(*splited);
            ++(*splited);
        }
        free(splited);
        if (charCharList.size >= 2) {
            CharList charListName = index_charcharlist(&charCharList, 0);
            CharList charListCount = index_charcharlist(&charCharList, 1);
            char *name = convert_arraychar(&charListName);
            char *temp_count = convert_arraychar(&charListCount);
            int count = atoi(temp_count);
            free(temp_count);
            while (count--) {
                ProductData product;
                product.name = name;
                product.producer_name = "";
                product.id = 0;
                product.data = "";
                append_productdatalist(&products, product);
            }
        }
        free_charcharlist(&charCharList);
    }
    free_charcharlist(&str);
    return products;
}

ProductDataList parseProductDataListProducer(char **arg) {
    CharCharList str = convert_CharCharList(arg);
    ProductDataList products = new_productdatalist(10);
    if (!(*arg)) {
        free_charcharlist(&str);
        return products;
    }
    for (int i = 0; i < str.size; i += 4) {
        if (i + 4 > str.size) {
            free_charcharlist(&str);
            return products;
        }
        CharList name = index_charcharlist(&str, i);
        CharList producer_name = index_charcharlist(&str, i + 1);
        CharList id = index_charcharlist(&str, i + 2);
        CharList data = index_charcharlist(&str, i + 3);
        ProductData product;
        product.name = convert_arraychar(&name);
        product.producer_name = convert_arraychar(&producer_name);
        char *temp = convert_arraychar(&id);
        product.id = atoi(temp);
        free(temp);
        product.data = convert_arraychar(&data);
        append_productdatalist(&products, product);
    }
    free_charcharlist(&str);
    return products;
}

int get_size(Warehouse *wh) {
    sem_wait(wh->mutex);
    int t = wh->products.size;
    sem_post(wh->mutex);
    return t;
}

Request parseRequest(char *str) {
    char **splited = str_split(str, ' ');
    Request request;
    if (!strncmp(*splited, "consumer", 8) && strlen(*splited) == 8)
        request.type = CONSUMER;
    else if (!strncmp(*splited, "producer", 8) && strlen(*splited) == 8)
        request.type = PRODUCER;
    else {
        request.type = ERROR;
        return request;
    }
    (splited)++;
    ProductDataList productDataList;
    if (request.type == CONSUMER)
        productDataList = parseProductDataListConsumer(splited);
    else
        productDataList = parseProductDataListProducer(splited);
    request.products = productDataList;
    return request;
}