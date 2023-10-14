//
// Created by alex on 10/14/23.
//

#include "main.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#endif

void sinit(stack *s) {
    s->alloc = 10;
    s->data = malloc(sizeof(elem *) * 10);
    if (s->data == NULL) {
        rerror("Out of memory!");
    }
    s->nextpos = 0;
}

void sfree(stack *s) {
    for (size_t i = 0; i < s->nextpos; i++) {
        free_elem(s->data[i]);
    }
    free(s->data);
}

void push(stack *s, elem *e) {
    if (s->nextpos >= s->alloc) {
        s->alloc *= 2;
        elem **new = realloc(s->data, sizeof(elem *) * s->alloc);
        if (new == NULL) {
            rerror("Out of memory!");
        }
        s->data = new;
    }
    s->data[s->nextpos++] = e;
}

elem *pop(stack *s) {
    if (s->nextpos == 0) {
        rerror("Stack underflow!");
    }
    if (s->nextpos <= s->alloc / 2) {
        s->alloc /= 2;
        elem **new = realloc(s->data, sizeof(elem *) * s->alloc);
        if (new == NULL) {
            rerror("Out of memory!");
        }
        s->data = new;
    }
    return s->data[--s->nextpos];
}

void push_number(stack *s, double number) {
    elem *e = new_elem(NUMBER);
    e->data.number = number;
    push(s, e);
}

void push_addr(stack *s, funptr ptr) {
    elem *e = new_elem(FUNPTR);
    e->data.ptr = ptr;
    push(s, e);
}

void push_array(stack *s, arr array) {
    elem *e = new_elem(ARRAY);
    e->data.array = array;
    push(s, e);
}

elem *peek(stack *s) {
    if (s->nextpos == 0) {
        rerror("Stack underflow!");
    }
    return s->data[s->nextpos - 1];
}

void dup(stack *s) {
    push(s, eclone(peek(s)));
}

void swap(stack *s) {
    elem *a = pop(s);
    elem *b = pop(s);
    push(s, a);
    push(s, b);
}

void over(stack *s) {
    elem *a = pop(s);
    elem *b = pop(s);
    push(s, b);
    push(s, a);
    push(s, eclone(b));
}

bool has_next(stack *s) {
    return s->nextpos > 0;
}