//
// Created by alex on 10/14/23.
//

#ifndef CUIUA_RUNTIME_OPERATORS_H
#define CUIUA_RUNTIME_OPERATORS_H

#include "main.h"

// calls the top element on the stack as a function
void call(stack *s);

// take the first n elements from an array
void take(stack *s);

// drop the first n elements from an array
void drop(stack *s);

// rotates an array by n elements
void rot(stack *s);

// reverses an array
void rev(stack *s);

// deshapes an array (makes the array one-dimensional)
void deshape(stack *s);

// checks if two arrays are the same
void match(stack *s);

// joins two arrays end-to-end
void join(stack *s);

// select elements from an array
// (the first element on the stack is a array of indecies)
// (the second element on the stack is the array to select from)
// (the result is pushed onto the stack)
void select_op(stack *s);

// gets element from an array at a index
void pick(stack *s);

// indexof   [x] [arr]  ->  [index]
// if x is a number, it returns the index of that number in the array (or -1 if it is not in the array)
// if x is an array, it does the indexof operation for each element in the array and returns a array of the resulting indecies
void indexof(stack *s);

// member   [x] [arr]  ->  [index]
// if x is a number, it returns true if that number is in the array
// if x is an array, it does the member operation for each element in the array and returns a array of the resulting booleans
void member(stack *s);

// returns the shape of an array (if it has a shape)
void shape(stack *s);

// repeats the given function n times
void repeat(stack *s);

// changes the shape of a flat array
// first arg: shape (array)
// second arg: array
void reshape(stack *s);

// executes the given function for each element in an array and returns a array of the results
void each(stack *s);

// puts the type of the top element on the stack
void type(stack *s);

// puts the length of the top element (array) on the stack
void len(stack *s);

// gets the first element of an array
void first(stack *s);

// make an array of all natural numbers from 0 to n
void range(stack *s);

// get indecies where array values are not equal to zero
void where(stack *s);

// remove all duplicate elements from an array
void deduplicate(stack *s);

// adds the top two elements on the stack
void add(stack *s);

// subtracts the top two elements on the stack
void sub(stack *s);

// divides the top two elements on the stack
void div_op(stack *s);

// multiplies the top two elements on the stack
void mul(stack *s);

// boxes the top element on the stack
void box(stack *s);

// unboxes the top element on the stack
void unbox(stack *s);

#endif //CUIUA_RUNTIME_OPERATORS_H
