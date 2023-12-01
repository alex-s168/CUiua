//
// Created by alex on 10/14/23.
//

#include "main.h"
#include "config.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#endif

void shutd();

void sinit(stack *s) {
    s->alloc = 10;
    s->data = malloc(sizeof(elem *) * 10);
    if (s->data == NULL) {
        rerror("Out of memory!");
    }
    s->nextpos = 0;
    add_shutdown_hook(shutd);
}

void sfree(stack *s) {
    for (size_t i = 0; i < s->nextpos; i++) {
#ifdef STACK_CLEANUP
        unused(s->data[i]);
#else
        free_elem(s->data[i]);
#endif
    }
    free(s->data);
}

void drop_one(stack *s) {
    free_elem(pop_f(s));
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

bool h_multiple_pop = false;

void shutd() {
    if (h_multiple_pop) {
        rerror("The multiple pop flag was not reset!");
    }
}

static void shorten_stack(stack *s) {
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
}

void hintes_multiple_pop(stack *s) {
    h_multiple_pop = true;
}

void hintds_multiple_pop(stack *s) {
    h_multiple_pop = false;
    shorten_stack(s);
}

elem *pop_f(stack *s) {
    if (s->nextpos == 0) {
        rerror("Stack underflow!");
    }
    if (!h_multiple_pop) {
        shorten_stack(s);
    }
    elem *e = s->data[--s->nextpos];
    unused(e);
    return e;
}

elem *pop(stack *s) {
    // return eclone(pop_f(s));
    return pop_f(s);
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
    push(s, e_from_arr(array));
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

// TODO: optimize
void push_strarr(stack *s, int len, char **arr) {
    new_array(s);
    for (int i = 0; i < len; i ++) {
      push_string(s, arr[i]);
    }
    end_array(s);
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

void dup2(stack *s) {
    if (s->nextpos < 2) {
        rerror("Stack underflow!");
    }
    elem *top = peek(s);
    elem *second = s->data[s->nextpos - 2];
    s->data[s->nextpos - 1] = eclone(second);
    s->data[s->nextpos++] = eclone(top);
}

void swap(stack *s) {
    if (s->nextpos < 2) {
        rerror("Stack underflow!");
    }
    elem *a = s->data[s->nextpos - 1];
    elem *b = s->data[s->nextpos - 2];
    s->data[s->nextpos - 1] = b;
    s->data[s->nextpos - 2] = a;
}

void over(stack *s) {
    if (s->nextpos < 2) {
        rerror("Stack underflow!");
    }
    push(s, eclone(s->data[s->nextpos - 2]));
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
