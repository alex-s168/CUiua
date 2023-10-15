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
#include <stdio.h>
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

// boxes the top element on the stack
void box(stack *s) {
    elem *e = peek(s);
    e->data.boxed = eclone(e);
    e->type = BOXED;
}

// unboxes the top element on the stack
void unbox(stack *s) {
    elem *e = peek(s);
    if (e->type != BOXED) {
        rerror("Expected boxed value, got %s!", type_to_str(e->type));
    }
    elem *unboxed = e->data.boxed;
    e->type = unboxed->type;
    e->data = unboxed->data;
    free(unboxed);
}

// debug prints the top element on the stack without popping it
void trace(stack *s) {
    elem *e = peek(s);
    printf("%s\n", etostr(e));
}

// pushes the number pi onto the stack
void do_pi(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = M_PI;
    push(s, e);
}

// pushes the number eta onto the stack
void do_eta(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = M_E;
    push(s, e);
}

// pushes the number tau onto the stack
void do_tau(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = M_PI * 2;
    push(s, e);
}

// pushes infinity onto the stack
void do_inf(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = INFINITY;
    push(s, e);
}

// pushes a random number between 0 and 1 onto the stack
void do_rand(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = (double) rand() / RAND_MAX;
    push(s, e);
}

// calls a function twice on one element each
void both(stack *s) {
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    elem *b = pop(s);
    elem *a = pop(s);

    push(s, a);
    f->data.ptr(s);

    push(s, b);
    f->data.ptr(s);
}

// temporarily pops the top element on the stack and executes the given function
void dip(stack *s) {
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    elem *e = pop(s);
    f->data.ptr(s);
    push(s, e);
}