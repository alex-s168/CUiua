//
// Created by alex on 10/14/23.
//

#include "main.h"
#include "operators.h"
#include "utils.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#endif

void call(stack *s) {
    elem *e = pop(s);
    if (e->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(e->type));
    }
    e->data.ptr(s);
}

// boxes the top element on the stack
void box(stack *s) {
    elem *e = peek(s);
    e->data.boxed = eclone(e);
    e->type = BOXED;
}

// unboxes the top element on the stack
void unbox(stack *s) {
    elem *e = peek(s);
    if (e->type != BOXED) {
        rerror("Expected boxed value, got %s!", type_to_str(e->type));
    }
    elem *unboxed = e->data.boxed;
    e->type = unboxed->type;
    e->data = unboxed->data;
    freex(unboxed);
}

// debug prints the top element on the stack without popping it
void trace(stack *s) {
    elem *e = peek(s);
    printf("%s\n", etostr(e));
}

// pushes the number pi onto the stack
void do_pi(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = M_PI;
    push(s, e);
}

// pushes the number eta onto the stack
void do_eta(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = M_E;
    push(s, e);
}

// pushes the number tau onto the stack
void do_tau(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = M_PI * 2;
    push(s, e);
}

// pushes infinity onto the stack
void do_inf(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = INFINITY;
    push(s, e);
}

// pushes a random number between 0 and 1 onto the stack
void do_rand(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = (double) rand() / RAND_MAX;
    push(s, e);
}

// calls a function twice on one element each
void both(stack *s) {
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    elem *b = pop(s);
    elem *a = pop(s);

    push(s, a);
    f->data.ptr(s);

    push(s, b);
    f->data.ptr(s);
}

// temporarily pops the top element on the stack and executes the given function
void dip(stack *s) {
    elem *f = pop(s);
    if (f->type != FUNPTR) {
        rerror("Expected function pointer, got %s!", type_to_str(f->type));
    }

    elem *e = pop(s);
    f->data.ptr(s);
    push(s, e);
}

// pushes true onto the stack
void true_op(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = 1;
    e->f_bool = true;
    push(s, e);
}

// pushes false onto the stack
void false_op(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = 0;
    e->f_bool = true;
    push(s, e);
}

// pushes NaN onto the stack
void nan_op(stack *s) {
    elem *e = new_elem(NUMBER);
    e->data.number = NAN;
    push(s, e);
}

static void not_rec(elem *e) {
    if (e->type == NUMBER) {
        e->data.number = !e->data.number;
        e->f_bool = true;
        return;
    }
    if (e->type == ARRAY) {
        arr array = e->data.array;
        for (size_t i = 0; i < array.len; i++) {
            not_rec(array.data[i]);
        }
        return;
    }
    if (e->type == BOXED) {
        not_rec(e->data.boxed);
        return;
    }
    rerror("Invalid type for not!");
}

// not operator
void not_op(stack *s) {
    not_rec(peek(s));
}

static void negate_rec(elem *e) {
    if (e->type == NUMBER) {
        e->data.number = -e->data.number;
        return;
    }
    if (e->type == ARRAY) {
        arr array = e->data.array;
        for (size_t i = 0; i < array.len; i++) {
            negate_rec(array.data[i]);
        }
        return;
    }
    if (e->type == BOXED) {
        negate_rec(e->data.boxed);
        return;
    }
    rerror("Invalid type for negate!");
}

// negates the top element on the stack
void negate(stack *s) {
    negate_rec(peek(s));
}

// checks if top two elements on the stack are equal
void eq(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    if (is_fraction(a) && is_fraction(b)) {
        push_bool(s, fract_equals(e_as_fraction(a), e_as_fraction(b)));
        return;
    }
    if (is_numeric(a) && is_numeric(b)) {
        push_bool(s, (e_as_num(a) == e_as_num(b)));
        return;
    }
    if (is_type(a) && is_type(b)) {
        push_bool(s, a->data.type == b->data.type);
        return;
    }
    rerror("Equals operator can only be used on numbers and types!");
}

// checks if top two elements on the stack are not equal
void neq(stack *s) {
    elem *b = pop(s);
    elem *a = pop(s);
    if (is_fraction(a) && is_fraction(b)) {
        push_bool(s, !fract_equals(e_as_fraction(a), e_as_fraction(b)));
        return;
    }
    if (is_numeric(a) && is_numeric(b)) {
        push_bool(s, (e_as_num(a) != e_as_num(b)));
        return;
    }
    if (is_type(a) && is_type(b)) {
        push_bool(s, a->data.type != b->data.type);
        return;
    }
    rerror("Not equals operator can only be used on numbers and types!");
}

// checks if top two elements on the stack are less than
void lt(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_fraction(a) && is_fraction(b)) {
        push_bool(s, fract_less(e_as_fraction(a), e_as_fraction(b)));
        return;
    }
    if (is_numeric(a) && is_numeric(b)) {
        push_bool(s, (e_as_num(a) < e_as_num(b)));
        return;
    }
    rerror("Less than operator can only be used on numbers!");
}

// checks if top two elements on the stack are less than or equal to
void lte(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_fraction(a) && is_fraction(b)) {
        push_bool(s, fract_less_equals(e_as_fraction(a), e_as_fraction(b)));
        return;
    }
    if (is_numeric(a) && is_numeric(b)) {
        push_bool(s, (e_as_num(a) <= e_as_num(b)));
        return;
    }
    rerror("Less than or equals operator can only be used on numbers!");
}

// checks if top two elements on the stack are greater than
void gt(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_fraction(a) && is_fraction(b)) {
        push_bool(s, fract_less(e_as_fraction(b), e_as_fraction(a)));
        return;
    }
    if (is_numeric(a) && is_numeric(b)) {
        push_bool(s, (e_as_num(a) > e_as_num(b)));
        return;
    }
    rerror("Greater than operator can only be used on numbers!");
}

// checks if top two elements on the stack are greater than or equal to
void gte(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_fraction(a) && is_fraction(b)) {
        push_bool(s, fract_less_equals(e_as_fraction(b), e_as_fraction(a)));
        return;
    }
    if (is_numeric(a) && is_numeric(b)) {
        push_bool(s, (e_as_num(a) >= e_as_num(b)));
        return;
    }
    rerror("Greater than or equals operator can only be used on numbers!");
}

// throws an error if the top element on the stack is not true
void assert(stack *s) {
    elem *e = pop(s);
    if (!is_bool(e) || !e_as_bool(e)) {
        rerror("Assertion error!");
    }
}

// max of two elements
void max_op(stack *s) {
    elem *ae = pop(s);
    elem *be = pop(s);
    if (!(is_numeric(ae) && is_numeric(be))) {
        rerror("Max operator can only be used on numbers!");
    }
    double a = e_as_num(ae);
    double b = e_as_num(be);
    if (a > b) {
        push_number(s, a);
    } else {
        push_number(s, b);
    }
}

// min of two elements
void min_op(stack *s) {
    elem *ae = pop(s);
    elem *be = pop(s);
    if (!(is_numeric(ae) && is_numeric(be))) {
        rerror("Min operator can only be used on numbers!");
    }
    double a = e_as_num(ae);
    double b = e_as_num(be);
    if (a < b) {
        push_number(s, a);
    } else {
        push_number(s, b);
    }
}

// call two functions on two elements
void bracket(stack *s) {
    elem *f1 = pop(s);
    if (!is_funptr(f1)) {
        rerror("Expected function, got %s!", type_to_str(f1->type));
    }

    elem *f2 = pop(s);
    if (!is_funptr(f2)) {
        rerror("Expected function, got %s!", type_to_str(f2->type));
    }

    elem *b = pop(s);
    elem *a = pop(s);

    push(s, a);
    e_as_funptr(f1)(s);

    push(s, b);
    e_as_funptr(f2)(s);
}

// reads all text from a file into a single string
void read_file(stack *s) {
    elem *name = pop_f(s);
    if (name->type != ARRAY) {
        rerror("Expected string (array), got %s!", type_to_str(name->type));
    }
    char *filename = arr_to_str(name->data.array);
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        rerror("Could not open file %s!", filename);
    }
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);
    char *str = malloc(len + 1);
    fread(str, 1, len, f);
    fclose(f);
    str[len] = '\0';
    push_string(s, str);
    free(filename);
    free(str);
    free_elem(name);
}

// writes a string to a file (overwrites / creates the file)
void write_file(stack *s) {
    elem *name = pop(s);
    if (name->type != ARRAY) {
        rerror("Expected string (array), got %s!", type_to_str(name->type));
    }

    elem *str = pop(s);
    if (str->type != ARRAY) {
        rerror("Expected string (array), got %s!", type_to_str(str->type));
    }

    char *filename = arr_to_str(name->data.array);
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        rerror("Could not open file %s!", filename);
    }
    char *str1 = arr_to_str(str->data.array);
    fputs(str1, f);
    fclose(f);
    free(filename);
    free(str1);
}

// for fractions:
//   converts it to an integer (equals to cast real and then floor)
// for numbers:
//   converts it to an integer (equals to floor)
// for strings:
//   parses the string as a integer (equals to real and then floor)
void cast_integer(stack *s) {
    elem *e = pop(s);
    if (is_numeric(e)) {
        push_number(s, floor(e_as_num(e)));
        return;
    }
    if (is_fraction(e)) {
        push_number(s, floor(fract_value(e_as_fraction(e))));
        return;
    }
    if (is_string(e)) {
        char *x;
        push_number(s, floor(strtod(e_as_str(e), &x)));
        return;
    }
    rerror("Cannot cast %s to integer!", type_to_str(e->type));
}

// for fractions:
//   error
// for numbers:
//   converts it to a string (negative sign gets appended in front of the string if the number is negative
// for strings:
//   does nothing
void cast_string(stack *s) {
    elem *e = pop(s);
    push_string(s, etostra(e, false));
}

// for fractions:
//   converts it to a real number
// for numbers:
//   does nothing
// for strings:
//   parses the string as a number
void cast_real(stack *s) {
    elem *e = pop(s);
    if (is_numeric(e)) {
        push_number(s, e_as_num(e));
        return;
    }
    if (is_string(e)) {
        char *x;
        push_number(s, strtod(e_as_str(e), &x));
        return;
    }
    if (is_fraction(e)) {
        push_number(s, fract_value(e_as_fraction(e)));
        return;
    }
    rerror("Cannot cast %s to real!", type_to_str(e->type));
}

// (ℝ)
// for numbers:
//   converts it to a fraction
// for strings:
//   error
// for fraction-like things:
//   converts it to a fraction
void cast_fraction(stack *s) {
    elem *e = pop(s);
    if (is_fraction(e)) {
        push(s, e_from_fraction(fract_shorten(e_as_fraction(e))));
        return;
    }
    if (is_numeric(e)) {
        push(s, e_from_fraction(to_fraction(e_as_num(e))));
        return;
    }
    rerror("Cannot cast %s to fraction!", type_to_str(e->type));
}

// [numerator] [denominator] fraction  ->  [numerator/denominator]
void makefract(stack *s) {
    elem *denom = pop(s);
    elem *num = pop(s);
    fract f = (fract) {
        .accuracy = 0,
        .numerator = (int) e_as_num(num),
        .denominator = (int) e_as_num(denom)
    };
    free_elem(denom);
    free_elem(num);
    push(s, e_from_fraction(f));
}

// returns the accuracy (in decimal places) of a number (or fraction) to another number (or fraction)
// [number] [other] accuracy  ->  [accuracy]
void accuracy(stack *s) {
    elem *other = pop(s);
    elem *num = pop(s);

    if (is_numeric(num) && is_numeric(other)) {
        // calculate accuracy of number to other
        double n = e_as_num(num);
        double o = e_as_num(other);
        if (n == o) {
            push_number(s, 0);
            return;
        }
        double diff = fabs(n - o);
        int acc = 0;
        while (diff < 1) {
            diff *= 10;
            acc++;
        }
        push_number(s, acc);
        return;
    }

    rerror("Cannot calculate accuracy of %s to %s!", type_to_str(num->type), type_to_str(other->type));
}

// or operator
void or_op(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_bool(a) && is_bool(b)) {
        push_bool(s, (e_as_bool(a) || e_as_bool(b)));
        return;
    }
    rerror("Or operator can only be used on booleans!");
}

// xor operator
void xor_op(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_bool(a) && is_bool(b)) {
        push_bool(s, (e_as_bool(a) != e_as_bool(b)));
        return;
    }
    rerror("Xor operator can only be used on booleans!");
}

// and operator
void and_op(stack *s) {
    elem *b = pop_f(s);
    elem *a = pop_f(s);
    if (is_bool(a) && is_bool(b)) {
        push_bool(s, (e_as_bool(a) && e_as_bool(b)));
        return;
    }
    rerror("And operator can only be used on booleans!");
}

// [bool] [fun] [fun] if
// executes the first function if the boolean is true, otherwise executes the second function
void if_op(stack *s) {
    elem *fTrue = pop(s);
    if (!is_funptr(fTrue)) {
        rerror("Expected function, got %s!", type_to_str(fTrue->type));
    }
    funptr ft = e_as_funptr(fTrue);

    elem *fFalse = pop(s);
    if (!is_funptr(fFalse)) {
        rerror("Expected function, got %s!", type_to_str(fFalse->type));
    }
    funptr ff = e_as_funptr(fFalse);

    elem *b = pop(s);
    if (!is_bool(b)) {
        rerror("Expected boolean, got %s!", type_to_str(b->type));
    }
    bool cond = e_as_bool(b);

    if (cond) {
        ft(s);
    } else {
        ff(s);
    }
}

// call two functions on one value
void fork_op(stack *s) {
    elem *f1 = pop(s);
    if (!is_funptr(f1)) {
        rerror("Expected function, got %s!", type_to_str(f1->type));
    }
    funptr f1t = e_as_funptr(f1);

    elem *f2 = pop(s);
    if (!is_funptr(f2)) {
        rerror("Expected function, got %s!", type_to_str(f2->type));
    }
    funptr f2t = e_as_funptr(f2);

    elem *e = pop_f(s);

    push(s, e);
    f1t(s);

    push(s, eclone(e));
    f2t(s);
}