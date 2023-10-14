//
// Created by alex on 10/14/23.
//

#include "main.h"

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
    if (e == NULL) {
        return;
    }
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
        case TYPE:
            return "type";
        default:
            return "unknown";
    }
}

char *etostr(elem *e) {
    switch (e->type) {
        case TYPE: {
            switch (e->data.type) {
                case NUMBER:
                    return "type(number)";
                case ARRAY:
                    return "type(array)";
                case FUNPTR:
                    return "type(function)";
                default:
                    return "type(unknown)";
            }
        }
        case NUMBER: {
            char *str = malloc(30);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            sprintf(str, "%f", e->data.number);
            // remove trailing zeros and dot (if possible)
            size_t len = strlen(str);
            while (len > 0 && str[len - 1] == '0') {
                len--;
            }
            if (len > 0 && str[len - 1] == '.') {
                len--;
            }
            str[len] = '\0';
            return str;
        }
        case ARRAY: {
            // first, calculate the length of the string
            size_t len = 2; // [[
            for (size_t i = 0; i < e->data.array.len; i++) {
                len += strlen(etostr(e->data.array.data[i]));
                len += 1; // space
            }
            if (e->data.array.len > 0) {
                len--; // no space after last element
            }
            len += 2; // ]]

            // allocate the string
            char *str = malloc(len);
            if (str == NULL) {
                rerror("Out of memory!");
            }

            // copy the string
            strcpy(str, "[");
            for (size_t i = 0; i < e->data.array.len; i++) {
                strcat(str, etostr(e->data.array.data[i]));
                if (i < e->data.array.len - 1) {
                    strcat(str, " ");
                }
            }
            strcat(str, "]");

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

bool elems_equal(elem *a, elem *b) {
    if (a->type != b->type) {
        return false;
    }
    switch (a->type) {
        case TYPE:
            return a->data.type == b->data.type;
        case NUMBER:
            return a->data.number == b->data.number;
        case ARRAY:
            if (a->data.array.len != b->data.array.len) {
                return false;
            }
            for (size_t i = 0; i < a->data.array.len; i++) {
                if (!elems_equal(a->data.array.data[i], b->data.array.data[i])) {
                    return false;
                }
            }
            return true;
        case FUNPTR:
            return a->data.ptr == b->data.ptr;
        default:
            return false;
    }
}

// deep-clones an element
elem *eclone(elem *e) {
    elem *clone = new_elem(e->type);
    switch (e->type) {
        case NUMBER:
            clone->data.number = e->data.number;
            break;
        case ARRAY:
            clone->data.array.len = e->data.array.len;
            clone->data.array.data = malloc(sizeof(elem *) * clone->data.array.len);
            if (clone->data.array.data == NULL) {
                rerror("Out of memory!");
            }
            for (size_t i = 0; i < clone->data.array.len; i++) {
                clone->data.array.data[i] = eclone(e->data.array.data[i]);
            }
            break;
        case FUNPTR:
            clone->data.ptr = e->data.ptr;
            break;
        default:
            rerror("Unknown element type %d", e->type);
    }
    return clone;
}