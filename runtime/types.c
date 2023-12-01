//
// Created by alex on 10/18/23.
//

// type conversion

#include "main.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <string.h>
#include <math.h>
#endif


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
                /* || (e->type == BOXED && is_array(e->data.boxed)); */;
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
    return (arr) {0};
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
    return (fract) {0};
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
    elem *e = e_from_num(c);
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