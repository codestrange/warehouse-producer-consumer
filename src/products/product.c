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
        char* chartemp = convert_arraychar(&temp);
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