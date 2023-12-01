//
// Created by alex on 10/18/23.
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

char *type_to_str(elem_type type) {
    switch (type) {
        case FRACTION:
            return "fraction";
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

__always_inline
char *etostr(elem *e) {
    return etostra(e, true);
}

char *numtostr(double num, bool negative_sign_right) {
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

char *etostra(elem *e, bool negative_sign_right) {
    switch (e->type) {
        case FRACTION: {
            char *str = malloc(100);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            str[0] = 0;
            strcat(str, numtostr(e->data.fraction.numerator, true));
            strcat(str, " ");
            strcat(str, numtostr(e->data.fraction.denominator, true));
            strcat(str, " ⑀");
            return str;
        }
        case BOXED: {
            char *str = malloc(30);
            if (str == NULL) {
                rerror("Out of memory!");
            }
            sprintf(str, "box(%s)", etostra(e->data.boxed, negative_sign_right));
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
                if (num == '\n') {
                    return "↪";
                }
                char *str = malloc(3);
                str[0] = '@';
                str[1] = (char) num;
                str[2] = '\0';
                return str;
            }

            return numtostr(num, negative_sign_right);
        }
        case ARRAY: {
            // first, calculate the length of the string
            size_t len = 1; // [
            size_t rlen = e->data.array.len;
            bool is_all_chars = (rlen > 0);
            for (size_t i = 0; i < rlen; i++) {
                elem *x = e->data.array.data[i];
                len += strlen(etostra(x, negative_sign_right));
                len += 1; // space
                if (x->f_char) {
                    continue;
                }
                is_all_chars = false;
            }
            if (rlen > 0) {
                len--; // no space after last element
            }
            len += 1; // ]\0

            if (is_all_chars) {
                char *str = malloc(rlen+3);

                // copy the string
                str[0] = '"';
                size_t i = 0;
                for (; i < rlen; i++) {
                    char c = (char) e->data.array.data[i]->data.number;
                    str[i + 1] = c;
                }
                str[i + 1] = '"';
                str[i + 2] = '\0';

                return str;
            }

            // allocate the string
            char *str = malloc(len);
            str[0] = '\0';
            if (str == NULL) {
                rerror("Out of memory!");
            }

            // copy the string
            strcpy(str, "[");
            for (size_t i = 0; i < e->data.array.len; i++) {
                strcat(str, etostra(e->data.array.data[i], negative_sign_right));
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
            return type_to_str(e->type);
        }
    }
}