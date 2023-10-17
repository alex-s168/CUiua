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
}

static fract add_f2(fract a, fract b) {
    return add_fract(a, b);
}

static double add_d2(double a, double b) {
    return a + b;
}

static fract add_f1d1(fract a, double b) {
    return add_fract_d(a, b);
}

static fract add_d1f1(double a, fract b) {
    return add_fract_d(b, a);
}

void add(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, combine_simple_two_numbers(a, b,
                                       add_f2, add_d2,
                                       add_f1d1, add_d1f1));
    free_elem(a);
    free_elem(b);
}

static fract sub_f2(fract a, fract b) {
    return sub_fract(a, b);
}

static double sub_d2(double a, double b) {
    return a - b;
}

static fract sub_f1d1(fract a, double b) {
    return sub_fract_d1(a, b);
}

static fract sub_d1f1(double a, fract b) {
    return sub_fract_d0(a, b);
}

void sub(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, combine_simple_two_numbers(a, b,
                                       sub_f2, sub_d2,
                                       sub_f1d1, sub_d1f1));
    free_elem(a);
    free_elem(b);
}

static fract mul_f2(fract a, fract b) {
    return mul_fract(a, b);
}

static double mul_d2(double a, double b) {
    return a * b;
}

static fract mul_f1d1(fract a, double b) {
    return mul_fract_d(a, b);
}

static fract mul_d1f1(double a, fract b) {
    return mul_fract_d(b, a);
}

void mul(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, combine_simple_two_numbers(a, b,
                                       mul_f2, mul_d2,
                                       mul_f1d1, mul_d1f1));
    free_elem(a);
    free_elem(b);
}

static fract div_f2(fract a, fract b) {
    return div_fract(a, b);
}

static double div_d2(double a, double b) {
    return a / b;
}

static fract div_f1d1(fract a, double b) {
    return div_fract_d1(a, b);
}

static fract div_d1f1(double a, fract b) {
    return div_fract_d0(a, b);
}

void div_op(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    push(s, combine_simple_two_numbers(a, b,
                                       div_f2, div_d2,
                                       div_f1d1, div_d1f1));
    free_elem(a);
    free_elem(b);
}