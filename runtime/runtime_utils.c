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
#include <string.h>
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
            array.data[len-1-i] = pop(s);
        }
    }
    e->type = ARRAY;
    e->data.array = array;
    push(s, e);
}


// ends the array literal and pushes it onto the stack (reverses array)
void end_array_rev(stack *s) {
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
            array.data[i - 1] = pop(s);
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

bool is_positive_int(elem *e) {
    return e->type == NUMBER && e->data.number == (int) e->data.number && e->data.number >= 0;
}

char *arr_to_str(arr a) {
    iarr ia = arr_to_iarr(a);
    char *str = malloc(ia.len + 1);
    if (str == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < ia.len; i++) {
        str[i] = (char) ia.data[i];
    }
    str[ia.len] = '\0';
    iarr_free(ia);
    return str;
}

arr str_to_arr(char *str) {
    size_t len = strlen(str);
    elem **data = malloc(len * sizeof(elem *));
    if (data == NULL) {
        rerror("Out of memory!");
    }
    for (size_t i = 0; i < len; i++) {
        elem *e = new_elem(NUMBER);
        e->data.number = str[i];
        e->f_char = true;
        data[i] = e;
    }
    return (arr) { .len = len, .data = data };
}