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
// if the array is too short for the shape, it padds with zeros
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
// equals to:
//   duplicate length range group
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

// Find the occurrences of one array in another
// Takes two arguments.
// First argument is an array or scalar of values to find.
// Second argument is an array to search in.
// If you want to search for an array in an array, you have to put it into an array like this: [[1 2 3]]
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

// checks if top two elements on the stack are equal
void eq(stack *s);

// checks if top two elements on the stack are not equal
void neq(stack *s);

// checks if top two elements on the stack are less than
void lt(stack *s);

// checks if top two elements on the stack are less than or equal to
void lte(stack *s);

// checks if top two elements on the stack are greater than
void gt(stack *s);

// checks if top two elements on the stack are greater than or equal to
void gte(stack *s);

// throws an error if the top element on the stack is not true
void assert(stack *s);

// max of two elements
void max_op(stack *s);

// min of two elements
void min_op(stack *s);

// first array is the array to be split
// second array is the array of indiecies to split at
// every split position will be removed from the array
// example 1:
//   [1 2 3 4 5 6 7 8 9] [3 6] split  ->  [[1 2 3] [5 6] [8 9]]
// split can be used to remove elements from an array at specific positions like this:
//   [1 2 3 4 5 6 7 8 9] [3 6] split deshape  ->  [1 2 3 5 6 8 9]
void split_op(stack *s);

// applies a function to each possible combination of two arrays
// example:
//   [1 2 3] [9 2 9 5] table  ->  [[10 3 10 6] [11 4 11 7] [12 5 12 8]]
void table(stack *s);

// combines two arrays by grouping each element of one array into a pair with the corresponding element of the other array
// if the arrays are not the same length, the shorter one is padded with zeros
// example:
//   [1 2 3] [4 5 6] group  ->  [[1 4] [2 5] [3 6]]
void group(stack *s);

// call two functions on two elements
void bracket(stack *s);

// pushes all elements of an array onto the stack
// if no array is given, it does nothing
void dearray(stack *s);

// removes a fragment from an array
// example:
//   [3 9 2 1 6] 1 3 fragment  ->  [3 6]
void fragment(stack *s);

// reads all text from a file into a single string
void read_file(stack *s);

// writes a string to a file (overwrites / creates the file)
void write_file(stack *s);

// for fractions:
//   error
// for numbers:
//   converts it to an integer (equals to floor)
// for strings:
//   parses the string as a integer (equals to real and then floor)
void cast_integer(stack *s);

// for fractions:
//   error
// for numbers:
//   converts it to a string (negative sign gets appended in front of the string if the number is negative
// for strings:
//   does nothing
void cast_string(stack *s);

// for fractions:
//   converts it to a real number
// for numbers:
//   does nothing
// for strings:
//   parses the string as a number
void cast_real(stack *s);

// for numbers:
//   converts it to a fraction
// for strings:
//   error
// for fraction-like things:
//   converts it to a fraction
void cast_fraction(stack *s);

// rotates the shape of an array
void transpose(stack *s);

// [numerator] [denominator] fraction  ->  [numerator/denominator]
void makefract(stack *s);

// returns the accuracy (in decimal places) of a number (or fraction) to another number (or fraction)
// [number] [other] accuracy  ->  [accuracy]
void accuracy(stack *s);

// or operator
void or_op(stack *s);

// xor operator
void xor_op(stack *s);

// and operator
void and_op(stack *s);

// equals to      each (and) reduce
void boolean_and_reduce(stack *s);

// counts the amount of elements in an array before the first false value
void count_until_false(stack *s);

// [bool] [fun] [fun] if
// executes the first function if the boolean is true, otherwise executes the second function
void if_op(stack *s);

#endif //CUIUA_RUNTIME_OPERATORS_H
