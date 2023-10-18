//
// Created by alex on 10/14/23.
//

#ifndef CUIUA_RUNTIME_H
#define CUIUA_RUNTIME_H

#include <stddef.h>
#include <stdbool.h>
#include "utils.h"
#include "fractions.h"

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
    BOXED,
    FRACTION
} elem_type;

char *type_to_str(elem_type type);

struct elem {
    union {
        double number;
        arr array;
        funptr ptr;
        elem_type type;
        elem *boxed;
        fract fraction;
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

// adds an element to the cleanup list
// do not call with an element pointer. first call alloc_address_e()
void add_for_cleanup(void *e);

// cleans all the elements in the cleanup list
void cleanup();

// frees a pointer
// if it is in the cleanup list it will be removed
void freex(void *e);

// returns the allocation address of an element
// DO NOT USE THIS FUNCTION OUTSIDE THE RUNTIME
void *alloc_address_e(elem *e);

// initializes the runtime
void initrt();

// stops the runtime
void stoprt();

// allocates a new element
elem *new_elem(elem_type type);

// frees a element
void free_elem(elem *e);

// copies a element into another one
void copy_elem(elem *dest, elem *src);

// starts a array literal
void new_array(stack *s);

// ends a array literal
void end_array(stack *s);

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

elem *pop_f(stack *s);

// reserves space for n elements
void sreserve(stack *s, size_t size);

// debug function
void stack_realloc(stack *s, size_t size);

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

// checks if two elements are equal
bool elems_equal(elem *a, elem *b);

// deep-clones an element
elem *eclone(elem *e);

// deep-clones an element into a memory location
void eclone_into(elem *dest, elem *src);

iarr arr_to_iarr(arr a);

char *arr_to_str(arr a);

arr str_to_arr(char *str);

bool is_positive_index(elem *e);



// does the element represent a number / is it a number?
bool is_numeric(elem *e);

// is the element an array?
bool is_array(elem *e);

// is the element a function pointer?
bool is_funptr(elem *e);

// is the element a boxed element?
bool is_boxed(elem *e);

// is the element a boolean?
bool is_bool(elem *e);

// is the element a character?
bool is_char(elem *e);

// is the element a string?
bool is_string(elem *e);

// is the element a type?
bool is_type(elem *e);

// is the element a fraction?
bool is_fraction(elem *e);



// gets the numeric value of an element
double e_as_num(elem *e);

// gets the boolean value of an element
bool e_as_bool(elem *e);

// gets the character value of an element
char e_as_char(elem *e);

// gets the string value of an element
char *e_as_str(elem *e);

// gets the array value of an element
arr e_as_arr(elem *e);

// gets the function pointer value of an element
funptr e_as_funptr(elem *e);

// gets the fraction value of an element
fract e_as_fraction(elem *e);


elem *e_from_num(double num);

elem *e_from_bool(bool b);

elem *e_from_char(char c);

elem *e_from_str(char *str);

elem *e_from_arr(arr array);

elem *e_from_funptr(funptr ptr);

elem *e_from_fraction(fract fraction);

elem *e_from_type(elem_type type);

elem *e_from_boxed(elem *e);

elem *e_from_iarr(iarr array);

#endif //CUIUA_RUNTIME_H
