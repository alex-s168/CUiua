//
// Created by alex on 10/17/23.
//

#include "operators.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#endif

static elem *combine_simple_two_numbers(
        elem *a,
        elem *b,
        fract (*f2op)(fract, fract),
        double (*d2op)(double, double),
        fract (*f1d1op)(fract, double),
        fract (*d1f1op)(double, fract)) {
    bool a_frac = is_fraction(a);
    bool b_frac = is_fraction(b);
    if (a_frac && b_frac) {
        return e_from_fraction(f2op(e_as_fraction(a), e_as_fraction(b)));
    }
    bool a_num = is_numeric(a);
    if (a_num && b_frac) {
        return e_from_fraction(d1f1op(e_as_num(a), e_as_fraction(b)));
    }
    bool b_num = is_numeric(b);
    if (a_frac && b_num) {
        return e_from_fraction(f1d1op(e_as_fraction(a), e_as_num(b)));
    }
    if (a_num && b_num) {
        return e_from_num(d2op(e_as_num(a), e_as_num(b)));
    }

    bool a_arr = is_array(a);
    if (a_arr && b_num) {
        arr array = e_as_arr(a);
        for (size_t i = 0; i < array.len; i++) {
            array.data[i] = combine_simple_two_numbers(array.data[i], b,
                                                       f2op, d2op,
                                                       f1d1op, d1f1op);
        }
        return e_from_arr(array);
    }
    bool b_arr = is_array(b);
    if (a_num && b_arr) {
        arr array = e_as_arr(b);
        for (size_t i = 0; i < array.len; i++) {
            array.data[i] = combine_simple_two_numbers(a, array.data[i],
                                                       f2op, d2op,
                                                       f1d1op, d1f1op);
        }
        return e_from_arr(array);
    }
    if (a_arr && b_arr) {
        arr a_array = e_as_arr(a);
        arr b_array = e_as_arr(b);
        if (a_array.len != b_array.len) {
            rerror("Cannot combine arrays of different lengths!");
        }
        for (size_t i = 0; i < a_array.len; i++) {
            a_array.data[i] = combine_simple_two_numbers(a_array.data[i], b_array.data[i],
                                                         f2op, d2op,
                                                         f1d1op, d1f1op);
        }
        return e_from_arr(a_array);
    }
    rerror("Invalid types for operator!");
    return NULL;
}

static double add_d2(double a, double b) {
    return a + b;
}

static fract add_d1f1(double a, fract b) {
    return add_fract_d(b, a);
}

void add(stack *s) {
    elem *b = pop_f(s);
    elem *a = peek(s);
    elem *e = combine_simple_two_numbers(a, b,
                                         add_fract, add_d2,
                                         add_fract_d, add_d1f1);
    copy_elem(a, e);
    free_elem(b);
}

static double sub_d2(double a, double b) {
    return a - b;
}

void sub(stack *s) {
    elem *b = pop_f(s);
    elem *a = peek(s);
    elem *e = combine_simple_two_numbers(a, b,
                                         sub_fract, sub_d2,
                                         sub_fract_d1, sub_fract_d0);
    copy_elem(a, e);
    free_elem(b);
}

static double mul_d2(double a, double b) {
    return a * b;
}

static fract mul_d1f1(double a, fract b) {
    return mul_fract_d(b, a);
}

void mul(stack *s) {
    elem *b = pop_f(s);
    elem *a = peek(s);
    elem *e = combine_simple_two_numbers(a, b,
                                         mul_fract, mul_d2,
                                         mul_fract_d, mul_d1f1);
    copy_elem(a, e);
    free_elem(b);
}

static double div_d2(double a, double b) {
    return a / b;
}

void div_op(stack *s) {
    elem *b = pop_f(s);
    elem *a = peek(s);
    elem *e = combine_simple_two_numbers(a, b,
                                         div_fract, div_d2,
                                         div_fract_d1, div_fract_d0);
    copy_elem(a, e);
    free_elem(b);
}