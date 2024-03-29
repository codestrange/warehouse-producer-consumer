#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

char **str_split(char *str, const char delim) {
    char **result = 0;
    int count = 0;
    char *tmp = str;
    char *last_comma = 0;
    char _delim[2];
    _delim[0] = delim;
    _delim[1] = 0;
    while (*tmp) {
        if (delim == *tmp) {
            ++count;
            last_comma = tmp;
        }
        ++tmp;
    }
    count += (last_comma < (str + strlen(str) - 1)) + 1;
    result = malloc(sizeof(char *) * (count + 5));
    char *_str = malloc(sizeof(char) * (strlen(str)) + 5);
    char *_tmp = _str;
    tmp = str;
    while (true) {
        *_tmp = *tmp;
        if (!*tmp)
            break;
        ++_tmp;
        ++tmp;
    }
    if (result) {
        int idx = 0;
        char *token = strtok(_str, _delim);
        while (token) {
            *(result + idx++) = strdup(token);
            token = strtok(0, _delim);
        }
        *(result + idx) = 0;
    }
    return result;
}