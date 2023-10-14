//
// Created by alex on 10/14/23.
//

#include "runtime.h"
#include "utils.h"

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

void call(stack *s) {
    elem *e = pop(s);
    if (e->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(e->type));
    }
    e->data.ptr(s);
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

// checks if two arrays are the same
// pushes 1 if true, 0 if false
// uses match_rec
void match(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    bool match = elems_equal(a, b);
    free_elem(a);
    free_elem(b);
    elem *e = new_elem(NUMBER);
    e->data.number = match ? 1 : 0;
    push(s, e);
}

// joins two arrays end-to-end
void join(stack *s) {
    elem *b = pop(s);
    if (b->type != ARRAY) {
        rerror("The second argument to join needs to be an array!");
    }
    elem *a = peek(s);
    if (a->type != ARRAY) {
        rerror("The first argument to join needs to be an array!");
    }

    arr a_arr = a->data.array;
    arr b_arr = b->data.array;

    if (a_arr.len == 0 && b_arr.len == 0) {
        return;
    }

    a_arr.data = realloc(a_arr.data, (a_arr.len + b_arr.len) * sizeof(elem *));
    if (a_arr.data == NULL) {
        rerror("Out of memory!");
    }

    for (size_t i = 0; i < b_arr.len; i++) {
        a_arr.data[a_arr.len + i] = b_arr.data[i];
    }
    a_arr.len += b_arr.len;
    a->data.array = a_arr;
}

// select elements from an array
// (the first element on the stack is a array of indecies)
// (the second element on the stack is the array to select from)
// (the result is pushed onto the stack)
void select_op(stack *s) {
    elem *b = pop(s);
    if (b->type != ARRAY) {
        rerror("The second argument to select needs to be an array!");
    }
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The first argument to select needs to be an array!");
    }
    arr array = a->data.array;
    arr indecies = b->data.array;
    arr new_array;
    new_array.len = indecies.len;
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < indecies.len; i++) {
        if (indecies.data[i]->type != NUMBER || round(indecies.data[i]->data.number) != indecies.data[i]->data.number) {
            rerror("The indecies need to be integers!");
        }
        size_t index = (size_t) indecies.data[i]->data.number;
        if (index >= array.len) {
            rerror("Index out of bounds!");
        }
        new_array.data[i] = array.data[index];
    }
    free(array.data);
    a->data.array = new_array;
    free_elem(b);
    push(s, a);
}

// gets element from an array at a index
void pick(stack *s) {
    elem *b = pop(s);
    if (b->type != NUMBER || round(b->data.number) != b->data.number) {
        rerror("The second argument to pick needs to be an integer!");
    }
    size_t index = (size_t) b->data.number;
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The first argument to pick needs to be an array!");
    }
    arr array = a->data.array;
    if (index >= array.len) {
        rerror("Index out of bounds!");
    }
    elem *e = array.data[index];
    free(array.data);
    a->data.array = array;
    free_elem(b);
    push(s, e);
}

static int index_of_elem_in_arr(elem *e, arr a) {
    for (size_t i = 0; i < a.len; i++) {
        if (elems_equal(e, a.data[i])) {
            return (int) i;
        }
    }
    return -1;
}

// indexof   [x] [arr]  ->  [index]
// if x is a number, it returns the index of that number in the array (or -1 if it is not in the array)
// if x is an array, it does the indexof operation for each element in the array and returns a array of the resulting indecies
void indexof(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The first argument to indexof needs to be an array!");
    }
    arr array = a->data.array;
    if (b->type == NUMBER || b-> type == TYPE) {
        int index = index_of_elem_in_arr(b, array);
        free_elem(a);
        free_elem(b);
        elem *e = new_elem(NUMBER);
        e->data.number = index;
        push(s, e);
    } else if (b->type == ARRAY) {
        arr indecies;
        indecies.len = b->data.array.len;
        indecies.data = malloc(indecies.len * sizeof(elem *));
        if (indecies.data == NULL) {
            rerror("Out of memory!");
        }
        for (size_t i = 0; i < indecies.len; i++) {
            if (b->data.array.data[i]->type != NUMBER || round(b->data.array.data[i]->data.number) != b->data.array.data[i]->data.number) {
                rerror("The indecies need to be integers!");
            }
            int index = index_of_elem_in_arr(b->data.array.data[i], array);
            elem *e = new_elem(NUMBER);
            e->data.number = index;
            indecies.data[i] = e;
        }
        free_elem(a);
        free_elem(b);
        elem *e = new_elem(ARRAY);
        e->data.array = indecies;
        push(s, e);
    } else {
        rerror("The second argument to indexof needs to be a number or an array!");
    }
}

// member   [x] [arr]  ->  [index]
// if x is a number, it returns true if that number is in the array
// if x is an array, it does the member operation for each element in the array and returns a array of the resulting booleans
// uses index_of_elem_in_arr
void member(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The first argument to member needs to be an array!");
    }
    arr array = a->data.array;
    if (b->type == NUMBER || b->type == TYPE) {
        int index = index_of_elem_in_arr(b, array);
        free_elem(a);
        free_elem(b);
        elem *e = new_elem(NUMBER);
        e->data.number = index == -1 ? 0 : 1;
        push(s, e);
    } else if (b->type == ARRAY) {
        arr indecies;
        indecies.len = b->data.array.len;
        indecies.data = malloc(indecies.len * sizeof(elem *));
        if (indecies.data == NULL) {
            rerror("Out of memory!");
        }
        for (size_t i = 0; i < indecies.len; i++) {
            if (b->data.array.data[i]->type != NUMBER || round(b->data.array.data[i]->data.number) != b->data.array.data[i]->data.number) {
                rerror("The indecies need to be integers!");
            }
            int index = index_of_elem_in_arr(b->data.array.data[i], array);
            elem *e = new_elem(NUMBER);
            e->data.number = index == -1 ? 0 : 1;
            indecies.data[i] = e;
        }
        free_elem(a);
        free_elem(b);
        elem *e = new_elem(ARRAY);
        e->data.array = indecies;
        push(s, e);
    } else {
        rerror("The second argument to member needs to be a number or an array!");
    }
}

static iarr shape_rec(arr a) {
    iarr shape;
    iarr_init(&shape, 1);
    shape.data[0] = a.len;

    if (a.len == 0) {
        return shape;
    }
    elem_type type = a.data[0]->type;
    for (size_t i = 1; i < a.len; i++) {
        if (a.data[i]->type != type) {
            rerror("Cannot get shape of array with different types!");
        }
    }
    if (type == ARRAY) {
        iarr sub_shape = shape_rec(a.data[0]->data.array);
        // check if shape is consistent
        for (size_t i = 0; i < a.len; i++) {
            iarr sub_shape2 = shape_rec(a.data[i]->data.array);
            if (sub_shape.len != sub_shape2.len) {
                rerror("Cannot get shape of array with inconsistent shape!");
            }
            for (size_t j = 0; j < sub_shape.len; j++) {
                if (sub_shape.data[j] != sub_shape2.data[j]) {
                    rerror("Cannot get shape of array with inconsistent shape!");
                }
            }
            free(sub_shape2.data);
        }
        shape = iarr_join(shape, sub_shape);
        free(sub_shape.data);
    }
    return shape;
}

// returns the shape of an array
// uses shape_rec
void shape(stack *s) {
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The argument to shape needs to be an array!");
    }
    arr array = a->data.array;
    iarr shape = shape_rec(array);
    arr new_array;
    new_array.len = shape.len;
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < shape.len; i++) {
        elem *e = new_elem(NUMBER);
        e->data.number = shape.data[i];
        new_array.data[i] = e;
    }
    free(shape.data);
    free(array.data);
    a->data.array = new_array;
    push(s, a);
}

// repeats the given function n times
void repeat(stack *s) {
    elem *n = pop(s);
    if (n->type != NUMBER || round(n->data.number) != n->data.number || n->data.number < 0) {
        rerror("The first argument to repeat needs to be a positive integer!");
    }
    size_t n_int = (size_t) n->data.number;
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("The second argument to repeat needs to be a function pointer!");
    }
    for (size_t i = 0; i < n_int; i++) {
        f->data.ptr(s);
    }
    free_elem(n);
    free_elem(f);
}

// executes the given function for each element in an array and returns a array of the results
void each(stack *s) {
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("The first argument to each needs to be a function pointer!");
    }
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The second argument to each needs to be an array!");
    }
    arr array = a->data.array;
    arr new_array;
    new_array.len = array.len;
    new_array.data = malloc(new_array.len * sizeof(elem *));
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < array.len; i++) {
        push(s, array.data[i]);
        f->data.ptr(s);
        new_array.data[i] = pop(s);
    }
    free(array.data);
    a->data.array = new_array;
    free_elem(f);
    push(s, a);
}

int reshape_rec(stack *s, iarr shape, arr *orig, size_t index) {
    if (shape.len == 0) {
        return 0;
    }
    if (shape.len == 1) {
        if (shape.data[0] > orig->len + index) {
            rerror("Cannot reshape array to a bigger size!");
        }
        new_array(s);
        size_t i;
        for (i = 0; i < shape.data[0]; i++) {
            elem *e = orig->data[index + i];
            push(s, e);
        }
        end_array(s);
        return i;
    }
    new_array(s);
    size_t used = 0;
    for (size_t i = 0; i < shape.data[0]; i++) {
        used += reshape_rec(s, (iarr) { .data = shape.data + 1, .len = shape.len - 1 }, orig, index + used);
    }
    end_array(s);
    return used;
}

// changes the shape of a flat array
// first arg: shape (array)
// second arg: array
void reshape(stack *s) {
    elem *a = pop(s);
    if (a->type != ARRAY) {
        rerror("The first argument to reshape needs to be an array (shape)!");
    }
    arr shape = a->data.array;

    elem *b = pop(s);
    if (b->type != ARRAY) {
        rerror("The second argument to reshape needs to be an array!");
    }
    arr array = b->data.array;

    iarr shape_int;
    shape_int.len = shape.len;
    shape_int.data = malloc(shape_int.len * sizeof(int));
    if (shape_int.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < shape.len; i++) {
        if (shape.data[i]->type != NUMBER || round(shape.data[i]->data.number) != shape.data[i]->data.number || shape.data[i]->data.number < 0) {
            rerror("The shape needs to be a array of positive integers!");
        }
        shape_int.data[i] = (int) shape.data[i]->data.number;
    }
    reshape_rec(s, shape_int, &array, 0);
    iarr_free(shape_int);
}

// puts the type of the top element on the stack
void type(stack *s) {
    elem *e = pop(s);
    elem *e2 = new_elem(TYPE);
    e2->data.type = e->type;
    push(s, e2);
}