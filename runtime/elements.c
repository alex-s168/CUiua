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
#include <math.h>
#endif

elem *new_elem(elem_type type) {
    elem *e = malloc(sizeof(elem));
    if (e == NULL) {
        rerror("Out of memory!");
    }
    e->type = type;
    e->f_bool = false;
    e->f_char = false;
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
    if (e->type == BOXED) {
        free_elem(e->data.boxed);
    }
    free(e);
}

char *type_to_str(elem_type type) {
    switch (type) {
        case BOXED:
            return "boxed";
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
    return etostra(e, true);
}

char *etostra(elem *e, bool negative_sign_right) {
    switch (e->type) {
        case BOXED: {
            char *str = malloc(30);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            sprintf(str, "box(%s)", etostr(e->data.boxed));
            return str;
        }
        case TYPE: {
            switch (e->data.type) {
                case BOXED:
                    return "type(boxed)";
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
            if (e->f_bool) {
                return e->data.number == 0 ? "⊥" : "⊤";
            }

            double num = e->data.number;
            if (e->f_char) {
                char *str = malloc(4);
                if (str == NULL) {
                    rerror("Out of memory!");
                }
                str[0] = '\'';
                str[1] = (char) num;
                str[2] = '\'';
                str[3] = '\0';
                return str;
            }

            if (num == INFINITY) {
                return "∞";
            }
            if (num == -INFINITY) {
                return "-∞";
            }
            if (isnan(fabs(num))) {
                return "♮";
            }
            if (num == M_PI) {
                return "π";
            }
            if (num == M_E) {
                return "η";
            }
            if (num == M_PI * 2) {
                return "τ";
            }
            char *str = malloc(50);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            bool neg = false;
            if (negative_sign_right) {
                neg = num < 0;
                if (neg) {
                    num = -num;
                }
                if (num == -0) {
                    num = 0;
                }
            }
            sprintf(str, "%f", num);
            // remove trailing zeros and dot (if possible)
            size_t len = strlen(str);
            while (len > 0 && str[len - 1] == '0') {
                len--;
            }
            if (len > 0 && str[len - 1] == '.') {
                len--;
            }
            str[len] = '\0';
            if (neg) {
                strcat(str, "¯");
            }
            return str;
        }
        case ARRAY: {
            // first, calculate the length of the string
            size_t len = 2; // [[
            size_t rlen = e->data.array.len;
            bool is_all_chars = (rlen > 0);
            for (size_t i = 0; i < rlen; i++) {
                elem *x = e->data.array.data[i];
                if (x->f_char) {
                    continue;
                }
                is_all_chars = false;
                len += strlen(etostr(x));
                len += 1; // space
            }
            if (rlen > 0) {
                len--; // no space after last element
            }
            len += 2; // ]]

            if (is_all_chars) {
                char *str = malloc(rlen + 3);

                // copy the string
                str[0] = '"';
                size_t i = 0;
                for (; i < rlen; i++) {
                    str[i + 1] = (char) e->data.array.data[i]->data.number;
                }
                str[i + 1] = '"';
                str[i + 2] = '\0';

                return str;
            }

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
        case BOXED:
            return elems_equal(a->data.boxed, b->data.boxed);
        case TYPE:
            return a->data.type == b->data.type;
        case NUMBER:
            if (isnan(a->data.number) && isnan(b->data.number)) {
                return true;
            }
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
        case BOXED:
            clone->data.boxed = eclone(e->data.boxed);
            break;
        case NUMBER:
            clone->data.number = e->data.number;
            clone->f_bool = e->f_bool;
            clone->f_char = e->f_char;
            break;
        case ARRAY:
            size_t len = e->data.array.len;
            clone->data.array.len = len;
            clone->data.array.data = malloc(sizeof(elem *) * len);
            if (clone->data.array.data == NULL) {
                rerror("Out of memory!");
            }
            for (size_t i = 0; i < len; i++) {
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