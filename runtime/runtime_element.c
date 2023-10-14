//
// Created by alex on 10/14/23.
//

#include "runtime.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

elem *new_elem(elem_type type) {
    elem *e = malloc(sizeof(elem));
    if (e == NULL) {
        rerror("Out of memory!");
    }
    e->type = type;
    return e;
}

void free_elem(elem *e) {
    if (e->type == ARRAY) {
        for (size_t i = 0; i < e->data.array.len; i++) {
            free_elem(e->data.array.data[i]);
        }
        free(e->data.array.data);
    }
    free(e);
}

char *type_to_str(elem_type type) {
    switch (type) {
        case NUMBER:
            return "number";
        case ARRAY:
            return "array";
        case FUNPTR:
            return "function";
        default:
            return "unknown";
    }
}

char *etostr(elem *e) {
    switch (e->type) {
        case NUMBER: {
            char *str = malloc(20);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            sprintf(str, "%f", e->data.number);
            return str;
        }
        case ARRAY: {
            char *str = malloc(20);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            str[0] = '[';
            size_t pos = 1;
            for (size_t i = 0; i < e->data.array.len; i++) {
                char *tmp = etostr(e->data.array.data[i]);
                size_t len = strlen(tmp);
                if (pos + len + 1 >= 20) {
                    str = realloc(str, pos + len + 1);
                    if (str == NULL) {
                        rerror("Out of memory!");
                    }
                }
                strcpy(str + pos, tmp);
                pos += len;
                str[pos++] = ',';
                free(tmp);
            }
            str[pos - 1] = ']';
            str[pos] = '\0';
            return str;
        }
        case FUNPTR: {
            char *str = malloc(20);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            sprintf(str, "function(%p)", e->data.ptr);
            return str;
        }
        default: {
            return "unknown";
        }
    }
}