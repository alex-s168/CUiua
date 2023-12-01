//
// Created by alex on 10/14/23.
//

#include "main.h"
#include "config.h"

#include <stdarg.h>

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#endif

void rerror(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Runtime error:\n");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void **cleanup_list;
size_t cleanup_list_len = 0;
size_t cleanup_list_alloc = 8;

stack array_builder_stack;

void add_for_cleanup(void *e) {
    if (cleanup_list_len >= cleanup_list_alloc) {
        cleanup_list_alloc *= 2;
        cleanup_list = realloc(cleanup_list, cleanup_list_alloc * sizeof(void *));
        if (cleanup_list == NULL) {
            rerror("Out of memory!");
        }
    }
    cleanup_list[cleanup_list_len++] = e;
}

// marks an element as unused
void unused(elem *e) {
    if (e == NULL) {
        return;
    }
    if (!e->is_alloc) {
        return;
    }
    add_for_cleanup(e);
}

void cleanup() {
    size_t j = 0;
    for (size_t i = 0; i < cleanup_list_len; i++) {
        __builtin_prefetch(cleanup_list + i);

        void *e = cleanup_list[i];
        if (e == NULL) {
            continue;
        }

        for (size_t k = i + 1; k < cleanup_list_len; k++) {
            if (cleanup_list[k] == e) {
                goto skip;
            }
        }

        free(e);
        j ++;

        skip:;
    }
    cleanup_list_len = 0;
    cleanup_list_alloc = 8;
    cleanup_list = realloc(cleanup_list, cleanup_list_alloc * sizeof(void *));
#ifdef CLEANUP_DEBUG
    printf("Cleaned up %zu elements\n", j);
#endif
}

// frees a pointer
// if it is in the cleanup list it will be removed
void freex(void *e) {
    if (e == NULL) {
        return;
    }

#ifdef FAST_FREE
    add_for_cleanup(e);
#else
    for (size_t i = 0; i < cleanup_list_len; i++) {
        if (cleanup_list[i] == e) {
            cleanup_list[i] = NULL;
        }
    }
    free(e);
#endif
}

// frees an element
// if it is in the cleanup list it will be removed
void freexe(elem *e) {
    if (e == NULL) {
        return;
    }
#ifdef FREEXE_LOG
    printf("freeing:\n  is_alloc: %d\n  type: %s\n", e->is_alloc, type_to_str(e->type));
#endif

    if (e->is_alloc) {
        freex(e);
    } else {
        if (e->type == ARRAY) {
            for (size_t i = 0; i < e->data.array.len; i++) {
                freexe(e->data.array.data[i]);
            }
            freex(e->data.array.data);
        } else if (e->type == BOXED) {
            freexe(e->data.boxed);
        }
        freex(e);
    }
}

vfun *shutdown_funs = NULL;
size_t shutdown_funs_count = 0;
size_t shutdown_funs_alloc = 8;

void initrt() {
    sinit(&array_builder_stack);

    srand(time(NULL));

    cleanup_list = malloc(cleanup_list_alloc * sizeof(void *));
    if (cleanup_list == NULL) {
        rerror("Out of memory!");
    }

    shutdown_funs = malloc(shutdown_funs_alloc * sizeof(vfun));
    if (shutdown_funs == NULL) {
        rerror("Out of memory!");
    }
}

// adds a function to be executed when the runtime stops
void add_shutdown_hook(vfun f) {
    if (shutdown_funs == NULL) {
        // no error here because the first sinit will call this function
        return;
    }

    for (size_t i = 0; i < shutdown_funs_count; i++) {
        if (shutdown_funs[i] == f) {
            return;
        }
    }

    if (shutdown_funs_count >= shutdown_funs_alloc) {
        shutdown_funs_alloc *= 2;
        shutdown_funs = realloc(shutdown_funs, shutdown_funs_alloc * sizeof(vfun));
        if (shutdown_funs == NULL) {
            rerror("Out of memory!");
        }
    }

    shutdown_funs[shutdown_funs_count++] = f;
}

void stoprt() {
    for (size_t i = 0; i < shutdown_funs_count; i++) {
        shutdown_funs[i]();
    }
    if (array_builder_stack.nextpos != 0) {
        rerror("Unmatched new_array()!");
    }
    sfree(&array_builder_stack);
#ifndef NO_CLEANUP
    cleanup();
#endif
    free(cleanup_list);
    free(shutdown_funs);
}

void new_array(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = s->nextpos;
    push(&array_builder_stack, e);
}

// ends the array literal and pushes it onto the stack
void end_array(stack *s) {
    if (array_builder_stack.nextpos == 0) {
        rerror("Unmatched end_array()!");
    }
    elem *e = pop_f(&array_builder_stack);
    size_t start = (size_t) e->data.number;
    size_t len = (size_t) (s->nextpos - start);
    arr array;
    if (len == 0 || start > s->nextpos) {
        array.data = NULL;
        array.len = 0;
    } else {
        array.data = malloc(len * sizeof(elem *));
        if (array.data == NULL) {
            rerror("Out of memory!");
        }
        array.len = len;
        hintes_multiple_pop(s);
        for (size_t i = 0; i < len; i++) {
            array.data[len-1-i] = pop_f(s);
        }
        hintds_multiple_pop(s);
        add_for_cleanup(array.data);
    }
    e->type = ARRAY;
    e->data.array = array;
    push(s, e);
}

void sdump(stack *s) {
    for (size_t i = 0; i < s->nextpos; i++) {
        printf("%s\n", etostr(s->data[i]));
    }
}

iarr arr_to_iarr(arr a) {
    iarr ia;
    ia.data = malloc(a.len * sizeof(int));
    if (ia.data == NULL) {
        rerror("Out of memory!");
    }
    ia.len = a.len;
    for (size_t i = 0; i < a.len; i++) {
        if (a.data[i]->type != NUMBER) {
            rerror("Expected number in array!");
        }
        ia.data[i] = (int) a.data[i]->data.number;
    }
    return ia;
}

void stack_realloc(stack *s, size_t size) {
    printf("reallocating stack to %zu\n", size);
}