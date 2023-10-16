//
// Created by alex on 10/14/23.
//

#ifndef CUIUA_RUNTIME_H
#define CUIUA_RUNTIME_H

#include <stddef.h>
#include <stdbool.h>
#include "utils.h"

// runtime error
void rerror(char *fmt, ...);

struct stack;
typedef struct stack stack;

// function pointer
typedef void (*funptr)(stack *);

struct elem;
typedef struct elem elem;

typedef struct {
    elem **data;
    size_t len;
} arr;

typedef enum {
    NUMBER,
    ARRAY,
    FUNPTR,
    TYPE,
    BOXED
} elem_type;

char *type_to_str(elem_type type);

struct elem {
    union {
        double number;
        arr array;
        funptr ptr;
        elem_type type;
        elem *boxed;
    } data;
    elem_type type;

    bool f_bool;
    bool f_char;
};

struct stack{
    elem **data;
    size_t nextpos;
    size_t alloc;
};

// initializes the runtime
void initrt();

// stops the runtime
void stoprt();

// allocates a new element
elem *new_elem(elem_type type);

// frees a element
void free_elem(elem *e);

// starts a array literal
void new_array(stack *s);

// ends a array literal
void end_array(stack *s);

// ends a array literal (reverses array)
void end_array_rev(stack *s);

// pushes a array onto the stack
void push_array(stack *s, arr array);

// pushes a number
void push_number(stack *s, double num);

// pushes a string
void push_string(stack *s, char *str);

// pushes a char
void push_char(stack *s, char c);

// pushes a boolean
void push_bool(stack *s, bool b);

// pushes a element onto the stack
void push(stack *s, elem *e);

bool has_next(stack *s);

// pops a element from the stack
elem *pop(stack *s);

// peeks at the top element on the stack
elem *peek(stack *s);

// pushes a function pointer onto the stack
void push_addr(stack *s, funptr ptr);

// duplicates the top element on the stack
void dup(stack *s);

// swaps the top two elements on the stack
void swap(stack *s);

// duplicate the second-to-top value to the top of the stack
void over(stack *s);

// initializes a stack
void sinit(stack *s);

// frees a stack
void sfree(stack *s);

// element to string
char *etostr(elem *e);

// element to string
char *etostra(elem *e, bool negative_sign_right);

// prints the whole stack
void sdump(stack *s);

bool elems_equal(elem *a, elem *b);

elem *eclone(elem *e);

iarr arr_to_iarr(arr a);

char *arr_to_str(arr a);

arr str_to_arr(char *str);

bool is_positive_int(elem *e);

#endif //CUIUA_RUNTIME_H
