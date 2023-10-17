//
// Created by alex on 10/14/23.
//

#include "main.h"
#include "operators.h"
#include "utils.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <math.h>
#endif


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
    new_array.data = malloc(1);
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < a.len; i++) {
        if (a.data[i]->type == ARRAY) {
            arr sub_array = deshape_rec(a.data[i]->data.array);
            new_array.len += sub_array.len;
            new_array.data = realloc(new_array.data, (new_array.len + 1) * sizeof(elem *));
            if (new_array.data == NULL) {
                rerror("Out of memory!");
            }
            for (size_t j = 0; j < sub_array.len; j++) {
                new_array.data[new_array.len - sub_array.len + j] = sub_array.data[j];
            }
            free(sub_array.data);
        } else {
            new_array.len++;
            new_array.data = realloc(new_array.data, (new_array.len + 1) * sizeof(elem *));
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
    e->f_bool = true;
    push(s, e);
}

// joins two arrays OR scalars end-to-end
void join(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    if (a->type == ARRAY && b->type == ARRAY) {
        arr array_a = a->data.array;
        arr array_b = b->data.array;
        arr new_array;
        new_array.len = array_a.len + array_b.len;
        new_array.data = malloc(new_array.len * sizeof(elem *));
        if (new_array.data == NULL) {
            rerror("Out of memory!");
        }
        for (size_t i = 0; i < array_a.len; i++) {
            new_array.data[i] = array_a.data[i];
        }
        for (size_t i = 0; i < array_b.len; i++) {
            new_array.data[array_a.len + i] = array_b.data[i];
        }
        free(array_a.data);
        free(array_b.data);
        a->data.array = new_array;
        push(s, a);
    } else if (a->type == ARRAY && b->type != ARRAY) {
        arr array_a = a->data.array;
        arr new_array;
        new_array.len = array_a.len + 1;
        new_array.data = malloc(new_array.len * sizeof(elem *));
        if (new_array.data == NULL) {
            rerror("Out of memory!");
        }
        for (size_t i = 0; i < array_a.len; i++) {
            new_array.data[i] = array_a.data[i];
        }
        new_array.data[array_a.len] = eclone(b);
        free(array_a.data);
        a->data.array = new_array;
        push(s, a);
    } else if (a->type != ARRAY && b->type == ARRAY) {
        arr array_b = b->data.array;
        arr new_array;
        new_array.len = array_b.len + 1;
        new_array.data = malloc(new_array.len * sizeof(elem *));
        if (new_array.data == NULL) {
            rerror("Out of memory!");
        }
        new_array.data[0] = eclone(a);
        for (size_t i = 0; i < array_b.len; i++) {
            new_array.data[i + 1] = array_b.data[i];
        }
        free(array_b.data);
        b->data.array = new_array;
        push(s, b);
    } else {
        arr new_array;
        new_array.len = 2;
        new_array.data = malloc(new_array.len * sizeof(elem *));
        if (new_array.data == NULL) {
            rerror("Out of memory!");
        }
        new_array.data[0] = eclone(a);
        new_array.data[1] = eclone(b);
        elem *e = new_elem(ARRAY);
        e->data.array = new_array;
        push(s, e);
    }
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
        new_array.data[i] = eclone(array.data[index]);
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
    if (b->type == NUMBER || b-> type == TYPE || b->type == BOXED) {
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
    if (b->type == NUMBER || b->type == TYPE || b->type == BOXED) {
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
        new_array(s);
        size_t i;
        for (i = 0; i < shape.data[0]; i++) {
            elem *e;
            if (i >= orig->len + index) {
                e = new_elem(NUMBER);
                e->data.number = 0;
            }
            else {
                e = orig->data[index + i];
            }
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
    arr array = deshape_rec(b->data.array);

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

// puts the length of the top element (array) on the stack
void len(stack *s) {
    elem *e = pop(s);
    size_t len = 1;
    if (e->type == ARRAY) {
        len = e->data.array.len;
    }
    elem *e2 = new_elem(NUMBER);
    e2->data.number = len;
    push(s, e2);
}

// gets the first element of an array
void first(stack *s) {
    elem *e = pop(s);
    if (e->type != ARRAY) {
        rerror("The argument to first needs to be an array!");
    }
    arr array = e->data.array;
    if (array.len == 0) {
        rerror("Cannot get first element of empty array!");
    }
    push(s, array.data[0]);
    free(array.data);
    free(e);
}

// makes an array of all natural numbers from 0 to n
void range(stack *s) {
    elem *e = pop(s);
    if (!is_positive_index(e)) {
        rerror("The argument to range needs to be a number!");
    }
    size_t n = (size_t) e_as_num(e);

    arr array;
    array.len = n;

    size_t off_into_alloc = sizeof(elem *) * n;
    void *alloc = malloc(off_into_alloc + sizeof(elem) * (n+1));

    array.data = (elem **) alloc;

    elem *data = (elem *) (alloc + off_into_alloc);
    if (data == NULL) {
        rerror("Out of memory!");
    }

    for (size_t i = 0; i < n; i++) {
        data[i].type = NUMBER;
        data[i].data.number = i;
        array.data[i] = &data[i];
    }

    elem *e2 = data + n;
    e2->type = ARRAY;
    e2->data.array = array;
    push(s, e2);
}

// get indecies where array values are not equal to zero
void where(stack *s) {
    elem *e = pop(s);
    if (e->type != ARRAY) {
        rerror("The argument to where needs to be an array!");
    }
    arr array = e->data.array;
    arr indecies;
    indecies.len = 0;
    indecies.data = malloc(0);
    if (indecies.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < array.len; i++) {
        if (array.data[i]->type != NUMBER) {
            rerror("The array needs to be a array of numbers!");
        }
        if (array.data[i]->data.number != 0) {
            indecies.len++;
            indecies.data = realloc(indecies.data, indecies.len * sizeof(elem *));
            if (indecies.data == NULL) {
                rerror("Out of memory!");
            }
            elem *ine = new_elem(NUMBER);
            ine->data.number = i;
            indecies.data[indecies.len - 1] = ine;
        }
    }
    free(array.data);
    e->data.array = indecies;
    push(s, e);
}

// remove all duplicate elements from an array
void deduplicate(stack *s) {
    elem *e = pop(s);
    if (e->type != ARRAY) {
        rerror("The argument to deduplicate needs to be an array!");
    }
    arr array = e->data.array;
    arr new_array;
    new_array.len = 0;
    new_array.data = malloc(0);
    if (new_array.data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < array.len; i++) {
        bool found = false;
        for (size_t j = 0; j < new_array.len; j++) {
            if (elems_equal(array.data[i], new_array.data[j])) {
                found = true;
                break;
            }
        }
        if (!found) {
            new_array.len++;
            new_array.data = realloc(new_array.data, new_array.len * sizeof(elem *));
            if (new_array.data == NULL) {
                rerror("Out of memory!");
            }
            new_array.data[new_array.len - 1] = array.data[i];
        }
    }
    free(array.data);
    e->data.array = new_array;
    push(s, e);
}

// enumerates an array
void enumerate(stack *s) {
    elem *e = pop(s);
    if (e->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(e->type));
    }
    arr array = e->data.array;

    new_array(s);
    for (size_t i = 0; i < array.len; i++) {
        new_array(s);
        push_number(s, i);
        push(s, eclone(array.data[i]));
        end_array(s);
    }
    end_array(s);

    free_elem(e);
}

// returns the indecies to the array as if it was sorted ascending
void sort_asc(stack *s) {
    elem *e = pop(s);
    if (e->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(e->type));
    }
    arr array = e->data.array;

    new_array(s);
    for (size_t i = 0; i < array.len; i++) {
        push_number(s, i);
    }
    end_array(s);

    // sort the indecies
    elem *f = pop(s);
    if (f->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(f->type));
    }
    arr indecies = f->data.array;

    for (size_t i = 0; i < indecies.len; i++) {
        for (size_t j = 0; j < indecies.len - i - 1; j++) {
            if (array.data[(size_t) indecies.data[j]->data.number]->type != NUMBER || array.data[(size_t) indecies.data[j + 1]->data.number]->type != NUMBER) {
                rerror("Expected array of numbers!");
            }
            if (array.data[(size_t) indecies.data[j]->data.number]->data.number > array.data[(size_t) indecies.data[j + 1]->data.number]->data.number) {
                elem *tmp = indecies.data[j];
                indecies.data[j] = indecies.data[j + 1];
                indecies.data[j + 1] = tmp;
            }
        }
    }

    push(s, f);
}

// returns the indecies to the array as if it was sorted descending
void sort_desc(stack *s) {
    elem *e = pop(s);
    if (e->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(e->type));
    }
    arr array = e->data.array;

    new_array(s);
    for (size_t i = 0; i < array.len; i++) {
        push_number(s, i);
    }
    end_array(s);

    // sort the indecies
    elem *f = pop(s);
    if (f->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(f->type));
    }
    arr indecies = f->data.array;

    for (size_t i = 0; i < indecies.len; i++) {
        for (size_t j = 0; j < indecies.len - i - 1; j++) {
            if (array.data[(size_t) indecies.data[j]->data.number]->type != NUMBER || array.data[(size_t) indecies.data[j + 1]->data.number]->type != NUMBER) {
                rerror("Expected array of numbers!");
            }
            if (array.data[(size_t) indecies.data[j]->data.number]->data.number < array.data[(size_t) indecies.data[j + 1]->data.number]->data.number) {
                elem *tmp = indecies.data[j];
                indecies.data[j] = indecies.data[j + 1];
                indecies.data[j + 1] = tmp;
            }
        }
    }

    push(s, f);
}

// Discard or copy some rows of an array
// Takes two arrays. The first array is the number of copies to keep of each row of the second array.
void keep(stack *s) {
    elem *a = pop(s);
    elem *b = pop(s);

    if (a->type != ARRAY || b->type != ARRAY) {
        rerror("Expected two arrays, got %s and %s!", type_to_str(a->type), type_to_str(b->type));
    }

    arr array_a = a->data.array;
    arr array_b = b->data.array;

    if (array_a.len != array_b.len) {
        rerror("Arrays must have the same length!");
    }

    new_array(s);
    for (size_t i = 0; i < array_a.len; i++) {
        if (array_a.data[i]->type != NUMBER) {
            rerror("Expected number, got %s!", type_to_str(array_a.data[i]->type));
        }
        if (array_a.data[i]->data.number < 0) {
            rerror("Number must be positive!");
        }
        for (size_t j = 0; j < array_a.data[i]->data.number; j++) {
            push(s, eclone(array_b.data[i]));
        }
    }
    end_array(s);
}

// Find the occurrences of one array in another
// Takes two arguments.
// First argument is an array or scalar of values to find.
// Second argument is an array to search in.
// If you want to search for an array in an array, you have to put it into an array like this: [[1 2 3]]
// example 1:
//   [1 8 5 2 3 5 4 5 6 7] 5 find  ->  [0 0 1 0 0 1 0 1 0 0]
// example 2:
//   [1 2 3 4 5 6 7 8 9 2 3 4] [2 3 4] find  ->  [0 1 0 0 0 0 0 0 0 1 0 0]
void find(stack *s) {
    elem *find = pop(s);

    elem *in = pop(s);
    if (in->type != ARRAY) {
        rerror("The second argument to find needs to be an array!");
    }

    if (find->type == ARRAY) {
        new_array(s);
        for (size_t i = 0; i < in->data.array.len; i ++) {
            bool any = false;
            elem *e = in->data.array.data[i];
            for (size_t j = 0; j < find->data.array.len; j ++) {
                if (elems_equal(e, find->data.array.data[j])) {
                    any = true;
                    break;
                }
            }
            push_bool(s, any);
        }
        end_array(s);
        return;
    }

    new_array(s);
    for (size_t i = 0; i < in->data.array.len; i ++) {
        push_bool(s, elems_equal(find, in->data.array.data[i]));
    }
    end_array(s);
}

// Combine two arrays OR scalars as rows of a new array
// example 1:
//   [1 2 3] [4 5 6] couple  ->  [[1 2 3] [4 5 6]]
// example 2:
//   1 [2 3 4] couple  ->  [[1] [2 3 4]]
void couple(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);

    new_array(s);

    if (a->type == ARRAY) {
        push(s, a);
        rev(s);
    }
    else {
        arr array;
        array.len = 1;
        array.data = malloc(sizeof(elem *));
        if (array.data == NULL) {
            rerror("Out of memory!");
        }
        array.data[0] = a;
        push_array(s, array);
    }

    if (b->type == ARRAY) {
        push(s, b);
        rev(s);
    }
    else {
        arr array;
        array.len = 1;
        array.data = malloc(sizeof(elem *));
        if (array.data == NULL) {
            rerror("Out of memory!");
        }
        array.data[0] = b;
        push_array(s, array);
    }

    end_array(s);
}

// Apply a function to a fixed value and each element of an array
void distribute(stack *s) {
    elem *f = pop(s);
    elem *c = pop(s);
    elem *b = pop(s);

    if (f->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    if (b->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(b->type));
    }

    arr array_b = b->data.array;

    new_array(s);
    for (size_t i = 0; i < array_b.len; i++) {
        push(s, c);
        push(s, array_b.data[i]);
        f->data.ptr(s);
    }
    end_array(s);
}

// Apply a reducing function to an array
void reduce(stack *s) {
    elem *f = pop(s);
    if (!is_funptr(f)) {
        rerror("Expected function, got %s!", type_to_str(f->type));
    }

    elem *a = pop(s);
    if (!is_array(a)) {
        rerror("Expected array, got %s!", type_to_str(a->type));
    }

    arr array = e_as_arr(a);

    if (array.len == 0) {
        rerror("Cannot reduce empty array!");
    }

    elem *e = array.data[0];
    push(s, e);
    funptr fptr = f->data.ptr;
    for (size_t i = 1; i < array.len; i++) {
        push(s, array.data[i]);
        fptr(s);
    }
    e = pop_f(s);

    free_elem(a);
    free_elem(f);
    push(s, e);
}

// Apply a reducing function to an array with an initial value
void fold(stack *s) {
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("Expected function, got %s!", type_to_str(f->type));
    }

    elem *i = pop(s);

    elem *b = pop(s);
    if (b->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(b->type));
    }

    arr array_b = b->data.array;

    if (array_b.len == 0) {
        rerror("Cannot fold empty array!");
    }

    elem *e = i;
    for (size_t i = 0; i < array_b.len; i++) {
        push(s, e);
        push(s, array_b.data[i]);
        f->data.ptr(s);
        e = pop(s);
    }

    free_elem(b);
    free_elem(f);
    push(s, e);
}

// Reduce, but keep intermediate values
// example:
//   [1 2 3 4 5] +scan  ->  [1 3 6 10 15]
void scan(stack *s) {
    elem *f = pop(s);
    elem *b = pop(s);

    if (f->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    if (b->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(b->type));
    }

    arr array_b = b->data.array;

    if (array_b.len == 0) {
        rerror("Cannot reduce empty array!");
    }

    new_array(s);
    elem *acc = eclone(array_b.data[0]);
    push(s, acc);
    for (size_t i = 1; i < array_b.len; i++) {
        push(s, acc);
        push(s, array_b.data[i]);
        f->data.ptr(s);
        acc = pop(s);
        push(s, acc);
    }
    end_array(s);

    free_elem(b);
    free_elem(f);
}

// first array is the array to be split
// second array is the array of indiecies to split at
// every split position will be removed from the array
// example 1:
//   [1 2 3 4 5 6 7 8 9] [3 6] split  ->  [[1 2 3] [5 6] [8 9]]
// split can be used to remove elements from an array at specific positions like this:
//   [1 2 3 4 5 6 7 8 9] [3 6] split deshape  ->  [1 2 3 5 6 8 9]
void split_op(stack *s) {
    elem *split = pop(s);
    if (split->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(split->type));
    }

    elem *array = pop(s);
    if (array->type != ARRAY) {
        rerror("Expected array, got %s!", type_to_str(array->type));
    }

    arr arr = array->data.array;

    iarr split_arr = arr_to_iarr(split->data.array);
    free_elem(split);

    new_array(s);

    new_array(s);
    for (size_t i = 0; i < arr.len; i ++) {
        if (iarr_contains(split_arr, i)) {
            end_array(s);
            new_array(s);
            continue;
        }
        push(s, arr.data[i]);
    }
    end_array(s);

    end_array(s);

    free(arr.data);
    free(array);
}

// applies a function to each possible combination of two arrays
// example:
//   [1 2 3] [9 2 9 5] (+) table  ->  [[10 3 10 6] [11 4 11 7] [12 5 12 8]]
void table(stack *s) {
    elem *f = pop(s);
    if (!is_funptr(f)) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    elem *a = pop(s);
    if (!is_array(a)) {
        rerror("Expected array, got %s!", type_to_str(a->type));
    }

    elem *b = pop(s);
    if (!is_array(b)) {
        rerror("Expected array, got %s!", type_to_str(b->type));
    }

    arr array_a = e_as_arr(a);
    arr array_b = e_as_arr(b);

    new_array(s);

    for (size_t i = 0; i < array_a.len; i++) {
        for (size_t j = 0; j < array_b.len; j++) {
            push(s, array_a.data[i]);
            push(s, array_b.data[j]);
            f->data.ptr(s);
        }
    }

    end_array(s);

    free(array_a.data);
    free(array_b.data);
    free(a);
    free(b);
}

// combines two arrays by grouping each element of one array into a pair with the corresponding element of the other array
// if the arrays are not the same length, the shorter one is padded with zeros
// example:
//   [1 2 3] [4 5 6] group  ->  [[1 4] [2 5] [3 6]]
void group(stack *s) {
    elem *a = pop(s);
    if (!is_array(a)) {
        rerror("Expected array, got %s!", type_to_str(a->type));
    }

    elem *b = pop(s);
    if (!is_array(b)) {
        rerror("Expected array, got %s!", type_to_str(b->type));
    }

    arr array_a = e_as_arr(a);
    arr array_b = e_as_arr(b);

    new_array(s);

    size_t len = array_a.len > array_b.len ? array_a.len : array_b.len;

    for (size_t i = 0; i < len; i++) {
        new_array(s);
        if (i < array_a.len) {
            push(s, array_a.data[i]);
        }
        else {
            push_number(s, 0);
        }
        if (i < array_b.len) {
            push(s, array_b.data[i]);
        }
        else {
            push_number(s, 0);
        }
        end_array(s);
    }

    end_array(s);

    free(array_a.data);
    free(array_b.data);
    free(a);
    free(b);
}

// pushes all elements of an array onto the stack
// if no array is given, it does nothing
void dearray(stack *s) {
    elem *e = peek(s);
    if (!is_array(e)) {
        return;
    }
    pop(s);

    arr array = e_as_arr(e);

    for (size_t i = 0; i < array.len; i++) {
        push(s, array.data[i]);
    }

    free(array.data);
    free(e);
}

// removes a fragment from an array
// example:
//   [3 9 2 1 6] 1 3 fragment  ->  [3 6]
void fragment(stack *s) {
    elem *end = pop(s);
    if (!is_positive_index(end)) {
        rerror("Expected positive integer, got %s!", type_to_str(end->type));
    }
    elem *start = pop(s);
    if (!is_positive_index(start)) {
        rerror("Expected positive integer, got %s!", type_to_str(start->type));
    }

    elem *array = pop(s);
    if (!is_array(array)) {
        rerror("Expected array, got %s!", type_to_str(array->type));
    }

    arr arr = e_as_arr(array);

    int nstart = (int) e_as_num(start);
    int nend = (int) e_as_num(end);

    if (nstart >= arr.len) {
        rerror("Start index out of bounds!");
    }

    if (nend >= arr.len) {
        rerror("End index out of bounds!");
    }

    if (nstart > nend) {
        rerror("Start index must be less than or equal to end index!");
    }

    new_array(s);
    for (size_t i = 0; i < arr.len; i++) {
        if (i < nstart || i > nend) {
            push(s, arr.data[i]);
        }
    }
    end_array(s);

    free(arr.data);
    free(array);
}

// rotates the shape of an array
void transpose(stack *s) {
    elem *arre = peek(s);
    if (arre->type != ARRAY) {
        rerror("Transpose can only be applied on multi-dimensional arrays!");
    }

    rerror("Transpose not implemented yet!");
}