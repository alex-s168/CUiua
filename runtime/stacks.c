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
        size_t all = s->alloc;
        elem **new = realloc(s->data, sizeof(elem *) * all);
        if (new == NULL) {
            rerror("Out of memory!");
        }
        s->data = new;
#ifdef STACK_DEBUG
        stack_realloc(s, all);
#endif
    }
    s->data[s->nextpos++] = e;
}

elem *pop_f(stack *s) {
    if (s->nextpos == 0) {
        rerror("Stack underflow!");
    }
    if (s->nextpos <= s->alloc / 2 && s->alloc > 10) {
        s->alloc /= 2;
        size_t all = s->alloc;

        elem **new = realloc(s->data, sizeof(elem *) * all);
        if (new == NULL) {
            rerror("Out of memory!");
        }
        s->data = new;
#ifdef STACK_DEBUG
        stack_realloc(s, all);
#endif
    }
    elem *e = s->data[--s->nextpos];
    // add_for_cleanup(e); // TODO: why segfault?
    return e;
}

elem *pop(stack *s) {
    return eclone(pop_f(s));
}

// reserves space for n elements
void sreserve(stack *s, size_t size) {
    size_t newsize = s->alloc + size;
    if (newsize > s->alloc) {
        elem **new = realloc(s->data, sizeof(elem *) * newsize);
        if (new == NULL) {
            rerror("Out of memory!");
        }
        s->data = new;
        s->alloc = newsize;
#ifdef STACK_DEBUG
        stack_realloc(s, newsize);
#endif
    }
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

// pushes a char
void push_char(stack *s, char c) {
    elem *e = new_elem(NUMBER);
    e->data.number = c;
    e->f_char = true;
    push(s, e);
}

void push_string(stack *s, char *str) {
    push_array(s, str_to_arr(str));
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
    elem *a = pop_f(s);
    elem *b = pop_f(s);
    push(s, a);
    push(s, b);
}

void over(stack *s) {
    elem *a = pop_f(s);
    elem *b = pop_f(s);
    push(s, b);
    push(s, a);
    push(s, eclone(b));
}

bool has_next(stack *s) {
    return s->nextpos > 0;
}

void push_bool(stack *s, bool b) {
    elem *e = new_elem(NUMBER);
    e->data.number = b;
    e->f_bool = true;
    push(s, e);
}