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

// enumerates an array
void enumerate(stack *s);

// returns the indecies to the array as if it was sorted ascending
void sort_asc(stack *s);

// returns the indecies to the array as if it was sorted descending
void sort_desc(stack *s);

// debug prints the top element on the stack without popping it
void trace(stack *s);

// pushes the number pi onto the stack
void do_pi(stack *s);

// pushes the number eta onto the stack
void do_eta(stack *s);

// pushes the number tau onto the stack
void do_tau(stack *s);

// pushes infinity onto the stack
void do_inf(stack *s);

// pushes a random number between 0 and 1 onto the stack
void do_rand(stack *s);

// calls a function twice on one element each
void both(stack *s);

// temporarily pops the top element on the stack and executes the given function
void dip(stack *s);

// Discard or copy some rows of an array
// Takes two arrays. The first array is the number of copies to keep of each row of the second array.
void keep(stack *s);

// Find the occurences of one array in another
// example 1:
//   [1 8 5 2 3 5 4 5 6 7] 5 find  ->  [0 0 1 0 0 1 0 1 0 0]
// example 2:
//   [1 2 3 4 5 6 7 8 9 2 3 4] [2 3 4] find  ->  [0 1 0 0 0 0 0 0 0 1 0 0]
void find(stack *s);

// Combine two arrays OR scalars as rows of a new array
// example 1:
//   [1 2 3] [4 5 6] couple  ->  [[1 2 3] [4 5 6]]
// example 2:
//   1 [2 3 4] couple  ->  [[1] [2 3 4]]
void couple(stack *s);

// Apply a function to a fixed value and each element of an array
void distribute(stack *s);

// Apply a reducing function to an array
void reduce(stack *s);

// Apply a reducing function to an array with an initial value
void fold(stack *s);

// Reduce, but keep intermediate values
// example:
//   [1 2 3 4 5] +scan  ->  [1 3 6 10 15]
void scan(stack *s);

// pushes true onto the stack
void true_op(stack *s);

// pushes false onto the stack
void false_op(stack *s);

// pushes NaN onto the stack
void nan_op(stack *s);

// not operator
void not_op(stack *s);

// negates the top element on the stack
void negate(stack *s);

#endif //CUIUA_RUNTIME_OPERATORS_H
