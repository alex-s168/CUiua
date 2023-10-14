//
// Created by alex on 10/14/23.
//

#include "main.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
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

stack array_builder_stack;

void initrt() {
    sinit(&array_builder_stack);
}

void stoprt() {
    if (array_builder_stack.nextpos != 0) {
        rerror("Unmatched new_array()!");
    }
    sfree(&array_builder_stack);
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
    array.data = malloc(len * sizeof(elem *));
    if (array.data == NULL) {
        rerror("Out of memory!");
    }
    array.len = len;
    for (size_t i = len; i > 0; i--) {
        array.data[i - 1] = pop(s);
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