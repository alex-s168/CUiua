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

            return numtostr(num, negative_sign_right);
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

// does the element represent a number / is it a number?
bool is_numeric(elem *e) {
    return      e->type == NUMBER
            ||  e->type == FRACTION
            || (e->type == BOXED && is_numeric(e->data.boxed));
}

// is the element an array?
bool is_array(elem *e) {
    return      e->type == ARRAY
            || (e->type == FRACTION)
            || (e->type == BOXED && is_array(e->data.boxed));
}

// is the element a function pointer?
bool is_funptr(elem *e) {
    return      e->type == FUNPTR
            || (e->type == BOXED && is_funptr(e->data.boxed));
}

// is the element a boxed element?
bool is_boxed(elem *e) {
    return e->type == BOXED;
}

// is the element a boolean?
bool is_bool(elem *e) {
    return is_numeric(e);
}

// is the element a character?
bool is_char(elem *e) {
    return is_numeric(e);
}

// is the element a string?
bool is_string(elem *e) {
    if (!is_array(e)) {
        return false;
    }
    arr a = e_as_arr(e);
    bool is_string = true;
    for (size_t i = 0; i < a.len; i++) {
        elem *x = a.data[i];
        if (!is_char(x)) {
            is_string = false;
            break;
        }
    }
    return is_string;
}

// is the element a type?
bool is_type(elem *e) {
    return      e->type == TYPE
            || (e->type == BOXED && is_type(e->data.boxed));
}

// is the element a fraction?
bool is_fraction(elem *e) {
    return      e->type == FRACTION
            || (e->type == BOXED
                && is_fraction(e->data.boxed))
            || (e->type == ARRAY
                && e->data.array.len == 2
                && is_numeric(e->data.array.data[0])
                && is_numeric(e->data.array.data[1]));
}

// gets the numeric value of an element
double e_as_num(elem *e) {
    if (e->type == NUMBER) {
        return e->data.number;
    }
    if (e->type == BOXED) {
        return e_as_num(e->data.boxed);
    }
    if (e->type == FRACTION) {
        return fract_value(e->data.fraction);
    }
    rerror("Cannot convert element to number!");
    return 0;
}

// gets the boolean value of an element
bool e_as_bool(elem *e) {
    if (!is_numeric(e)) {
        rerror("Cannot convert element to boolean!");
    }
    return (bool) e_as_num(e);
}

// gets the character value of an element
char e_as_char(elem *e) {
    if (!is_numeric(e)) {
        rerror("Cannot convert element to character!");
    }
    return (char) e_as_num(e);
}

// gets the string value of an element
char *e_as_str(elem *e) {
    return arr_to_str(e_as_arr(e));
}

// gets the array value of an element
arr e_as_arr(elem *e) {
    if (e->type == ARRAY) {
        return e->data.array;
    }
    if (e->type == BOXED) {
        return e_as_arr(e->data.boxed);
    }
    if (e->type == FRACTION) {
        return (arr) {
            .len = 2,
            .data = (elem *[]) {
                e_from_num(e->data.fraction.numerator),
                e_from_num(e->data.fraction.denominator)
            }
        };
    }
    rerror("Cannot convert element to array!");
}

// gets the function pointer value of an element
funptr e_as_funptr(elem *e) {
    if (e->type == FUNPTR) {
        return e->data.ptr;
    }
    if (e->type == BOXED) {
        return e_as_funptr(e->data.boxed);
    }
    rerror("Cannot convert element to function pointer!");
    return NULL;
}

// gets the fraction value of an element
fract e_as_fraction(elem *e) {
    if (e->type == FRACTION) {
        return e->data.fraction;
    }
    if (e->type == BOXED) {
        return e_as_fraction(e->data.boxed);
    }
    if (e->type == NUMBER) {
        return to_fraction(e->data.number);
    }
    if (e->type == ARRAY) {
        arr a = e_as_arr(e);
        if (a.len != 2) {
            rerror("Cannot convert array to fraction (Only arrays consisting of two numbers can be converted to fraction)!");
        }
        return (fract) {
            .numerator = e_as_num(a.data[0]),
            .denominator = e_as_num(a.data[1])
        };
    }
    rerror("Cannot convert element to fraction!");
}

elem *e_from_num(double num) {
    elem *e = new_elem(NUMBER);
    e->data.number = num;
    return e;
}

elem *e_from_bool(bool b) {
    elem *e = new_elem(NUMBER);
    e->data.number = b ? 1 : 0;
    e->f_bool = true;
    return e;
}

elem *e_from_char(char c) {
    elem *e = new_elem(NUMBER);
    e->data.number = c;
    e->f_char = true;
    return e;
}

arr str_to_arr(char *str) {
    size_t len = strlen(str);
    elem **data = malloc(sizeof(elem *) * len);
    if (data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < len; i++) {
        data[i] = e_from_char(str[i]);
    }
    return (arr) {
        .len = len,
        .data = data
    };
}

elem *e_from_str(char *str) {
    return e_from_arr(str_to_arr(str));
}

elem *e_from_arr(arr array) {
    elem *e = new_elem(ARRAY);
    e->data.array.len = array.len;
    e->data.array.data = malloc(sizeof(elem *) * array.len);
    if (e->data.array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < array.len; i++) {
        e->data.array.data[i] = array.data[i];
    }
    return e;
}

elem *e_from_funptr(funptr ptr) {
    elem *e = new_elem(FUNPTR);
    e->data.ptr = ptr;
    return e;
}

elem *e_from_fraction(fract fraction) {
    elem *e = new_elem(FRACTION);
    e->data.fraction = fract_shorten(fraction);
    return e;
}

elem *e_from_type(elem_type type) {
    elem *e = new_elem(TYPE);
    e->data.type = type;
    return e;
}

elem *e_from_boxed(elem *e) {
    elem *box = new_elem(BOXED);
    box->data.boxed = e;
    return box;
}

elem *e_from_iarr(iarr array) {
    elem *e = new_elem(ARRAY);
    e->data.array.len = array.len;
    e->data.array.data = malloc(sizeof(elem *) * array.len);
    if (e->data.array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < array.len; i++) {
        e->data.array.data[i] = e_from_num(array.data[i]);
    }
    return e;
}

char *arr_to_str(arr a) {
    iarr ia = arr_to_iarr(a);
    char *str = malloc(ia.len + 1);
    if (str == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < ia.len; i++) {
        str[i] = (char) ia.data[i];
    }
    str[ia.len] = '\0';
    iarr_free(ia);
    return str;
}

bool is_positive_index(elem *e) {
    if (!is_numeric(e)) {
        return false;
    }
    double num = e_as_num(e);
    return num >= 0 && num == floor(num);
}