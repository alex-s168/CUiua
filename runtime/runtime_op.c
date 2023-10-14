//
// Created by alex on 10/14/23.
//

#include "runtime.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <math.h>
#endif

// TODO: after popping an element and not using it anymore, free it

static elem *op_on_all_elem(elem *a, elem *b, elem *(*op)(elem *, elem *)) {
    // possible combinations:
    //   number, number
    //   number, array
    //   array, number
    //   array, array

    if (a->type == NUMBER && b->type == NUMBER) {
        return op(a, b);
    }
    if (a->type == NUMBER && b->type == ARRAY) {
        arr array = b->data.array;
        for (size_t i = 0; i < array.len; i++) {
            array.data[i] = op(a, array.data[i]);
        }
        return b;
    }
    if (a->type == ARRAY && b->type == NUMBER) {
        arr array = a->data.array;
        for (size_t i = 0; i < array.len; i++) {
            array.data[i] = op(array.data[i], b);
        }
        return a;
    }
    if (a->type == ARRAY && b->type == ARRAY) {
        arr array_a = a->data.array;
        arr array_b = b->data.array;
        if (array_a.len != array_b.len) {
            rerror("Arrays must have the same length!");
        }
        for (size_t i = 0; i < array_a.len; i++) {
            array_a.data[i] = op(array_a.data[i], array_b.data[i]);
        }
        return a;
    }
    if (a->type == FUNPTR || b->type == FUNPTR) {
        rerror("Pointer math is not permitted!");
        return NULL;
    }

    rerror("Invalid types for operation!");
}

static elem *add_elem(elem *a, elem *b) {
    if (a->type != NUMBER || b->type != NUMBER) {
        return op_on_all_elem(a, b, add_elem);
    }
    elem *e = new_elem(NUMBER);
    e->data.number = a->data.number + b->data.number;
    return e;
}

static elem *sub_elem(elem *a, elem *b) {
    if (a->type != NUMBER || b->type != NUMBER) {
        return op_on_all_elem(a, b, sub_elem);
    }
    elem *e = new_elem(NUMBER);
    e->data.number = a->data.number - b->data.number;
    return e;
}

static elem *div_elem(elem *a, elem *b) {
    if (a->type != NUMBER || b->type != NUMBER) {
        return op_on_all_elem(a, b, div_elem);
    }
    elem *e = new_elem(NUMBER);
    e->data.number = a->data.number / b->data.number;
    return e;
}

static elem *mul_elem(elem *a, elem *b) {
    if (a->type != NUMBER || b->type != NUMBER) {
        return op_on_all_elem(a, b, mul_elem);
    }
    elem *e = new_elem(NUMBER);
    e->data.number = a->data.number * b->data.number;
    return e;
}

void add(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, add_elem(a, b));
}

void sub(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, sub_elem(a, b));
}

void div_op(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, div_elem(a, b));
}

void mul(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, mul_elem(a, b));
}

// ↙
void take(stack *s) {
    elem *n = pop(s);
    if (n->type != NUMBER || round(n->data.number) != n->data.number || n->data.number < 0) {
        rerror("The first argument to take needs to be a positive integer!");
    }
    size_t n_int = (size_t) n->data.number;
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The second argument to take needs to be an array!");
    }
    arr array = a->data.array;
    if (n->data.number > array.len) {
        rerror("Amount bigger than array length!");
    }
    arr new_array;
    new_array.len = n_int;
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < new_array.len; i++) {
        new_array.data[i] = array.data[i];
    }
    free(array.data);
    a->data.array = new_array;
    push(s, a);
    free_elem(n);
}

// ↘
// removes the first n elements from an array
void drop(stack *s) {
    elem *n = pop(s);
    if (n->type != NUMBER || round(n->data.number) != n->data.number || n->data.number < 0) {
        rerror("The first argument to drop needs to be a positive integer!");
    }
    size_t n_int = (size_t) n->data.number;
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The second argument to drop needs to be an array!");
    }
    arr array = a->data.array;
    if (n->data.number > array.len) {
        rerror("Amount bigger than array length!");
    }
    arr new_array;
    new_array.len = (size_t) (array.len - n_int);
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < new_array.len; i++) {
        new_array.data[i] = array.data[i + n_int];
    }
    free(array.data);
    a->data.array = new_array;
    push(s, a);
    free_elem(n);
}

// rotates an array by n elements (↻)
void rot(stack *s) {
    elem *n = pop(s);
    if (n->type != NUMBER || round(n->data.number) != n->data.number) {
        rerror("The first argument to rot needs to be an integer!");
    }
    size_t n_int = (size_t) n->data.number;
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The second argument to rot needs to be an array!");
    }
    arr array = a->data.array;
    arr new_array;
    new_array.len = array.len;
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < new_array.len; i++) {
        new_array.data[i] = array.data[(i + n_int) % array.len];
    }
    free(array.data);
    a->data.array = new_array;
    push(s, a);
    free_elem(n);
}

// reverses an array (⇌)
void rev(stack *s) {
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The argument to rev needs to be an array!");
    }
    arr array = a->data.array;
    arr new_array;
    new_array.len = array.len;
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < new_array.len; i++) {
        new_array.data[i] = array.data[array.len - i - 1];
    }
    free(array.data);
    a->data.array = new_array;
    push(s, a);
}

// deshapes an array (makes the array one-dimensional)
// array can be of any shape (depth)
// recursively flattens the array
static arr deshape_rec(arr a) {
    arr new_array;
    new_array.len = 0;
    new_array.data = malloc(0);
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < a.len; i++) {
        if (a.data[i]->type == ARRAY) {
            arr sub_array = deshape_rec(a.data[i]->data.array);
            new_array.len += sub_array.len;
            new_array.data = realloc(new_array.data, new_array.len * sizeof(elem *));
            if (new_array.data == NULL) {
                rerror("Out of memory!");
            }
            for (size_t j = 0; j < sub_array.len; j++) {
                new_array.data[new_array.len - sub_array.len + j] = sub_array.data[j];
            }
            free(sub_array.data);
        } else {
            new_array.len++;
            new_array.data = realloc(new_array.data, new_array.len * sizeof(elem *));
            if (new_array.data == NULL) {
                rerror("Out of memory!");
            }
            new_array.data[new_array.len - 1] = a.data[i];
        }
    }
    free(a.data);
    return new_array;
}

// uses deshape_rec
void deshape(stack *s) {
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The argument to deshape needs to be an array!");
    }
    arr array = a->data.array;
    arr new_array = deshape_rec(array);
    a->data.array = new_array;
    push(s, a);
}