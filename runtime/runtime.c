//
// Created by alex on 10/14/23.
//

#include "main.h"

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

void cleanup() {
    for (size_t i = 0; i < cleanup_list_len; i++) {
        void *e = cleanup_list[i];
        if (e == NULL) {
            continue;
        }

        // check if somewhere else in the list is a pointer to the same memory
        for (size_t j = i+1; j < cleanup_list_len; j++) {
            if (cleanup_list[j] == e) {
                cleanup_list[j] = NULL;
            }
        }

        free(e);
    }
    cleanup_list_len = 0;
}

// frees a pointer
// if it is in the cleanup list it will be removed
void freex(void *e) {
    for (size_t i = 0; i < cleanup_list_len; i++) {
        if (cleanup_list[i] == e) {
            cleanup_list[i] = NULL;
            break;
        }
    }
    free(e);
}

void initrt() {
    sinit(&array_builder_stack);
    srand(time(NULL));
    cleanup_list = malloc(cleanup_list_alloc * sizeof(void *));
}

void stoprt() {
    if (array_builder_stack.nextpos != 0) {
        rerror("Unmatched new_array()!");
    }
    sfree(&array_builder_stack);
    cleanup();
    free(cleanup_list);
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
    elem *e = pop(&array_builder_stack);
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
        for (size_t i = 0; i < len; i++) {
            array.data[len-1-i] = pop_f(s);
        }
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