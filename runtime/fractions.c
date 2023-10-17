//
// Created by alex on 10/17/23.
//

#include "fractions.h"

#include <math.h>
#include <stdlib.h>

fract fract_shorten(fract a) {
    int gcd = 1;
    for (int i = 1; i <= a.numerator && i <= a.denominator; i++) {
        if (a.numerator % i == 0 && a.denominator % i == 0) {
            gcd = i;
        }
    }
    a.numerator /= gcd;
    a.denominator /= gcd;
    return a;
}

// tries to find the most accurate representation of a double as a fraction
fract to_fraction(double d) {
    fract f;
    f.numerator = 1;
    f.denominator = 1;
    f.accuracy = 0;
    double error = fabs(d - fract_value(f));
    for (int i = 1; i < 100; i++) {
        for (int j = 1; j < 100; j++) {
            fract f2;
            f2.numerator = i;
            f2.denominator = j;
            f2.accuracy = 0;
            double error2 = fabs(d - fract_value(f2));
            if (error2 < error) {
                f = f2;
                error = error2;
            }
        }
    }
    return f;
}

fract add_fract(fract a, fract b) {
    fract f;
    f.numerator = a.numerator * b.denominator + b.numerator * a.denominator;
    f.denominator = a.denominator * b.denominator;
    f.accuracy = a.accuracy > b.accuracy ? a.accuracy : b.accuracy;
    return f;
}

fract add_fract_d(fract a, double b) {
    fract f = to_fraction(b);
    return add_fract(a, f);
}

fract sub_fract(fract a, fract b) {
    fract f;
    f.numerator = a.numerator * b.denominator - b.numerator * a.denominator;
    f.denominator = a.denominator * b.denominator;
    f.accuracy = a.accuracy > b.accuracy ? a.accuracy : b.accuracy;
    return f;
}
fract sub_fract_d0(double a, fract b) {
    fract f = to_fraction(a);
    return sub_fract(f, b);
}
fract sub_fract_d1(fract a, double b) {
    fract f = to_fraction(b);
    return sub_fract(a, f);
}

fract mul_fract(fract a, fract b) {
    fract f;
    f.numerator = a.numerator * b.numerator;
    f.denominator = a.denominator * b.denominator;
    f.accuracy = a.accuracy + b.accuracy;
    return f;
}
fract mul_fract_d(fract a, double b) {
    fract f = to_fraction(b);
    return mul_fract(a, f);
}

fract div_fract(fract a, fract b) {
    fract f;
    f.numerator = a.numerator * b.denominator;
    f.denominator = a.denominator * b.numerator;
    f.accuracy = a.accuracy + b.accuracy;
    return f;
}
fract div_fract_d0(double a, fract b) {
    fract f = to_fraction(a);
    return div_fract(f, b);
}
fract div_fract_d1(fract a, double b) {
    fract f = to_fraction(b);
    return div_fract(a, f);
}

fract pow_fract(fract a, fract b) {
    fract f;
    f.numerator = pow(a.numerator, b.numerator);
    f.denominator = pow(a.denominator, b.denominator);
    f.accuracy = a.accuracy * b.accuracy;
    return f;
}
fract pow_fract_d0(double a, fract b) {
    fract f = to_fraction(a);
    return pow_fract(f, b);
}
fract pow_fract_d1(fract a, double b) {
    fract f = to_fraction(b);
    return pow_fract(a, f);
}

fract neg_fract(fract a) {
    fract f;
    f.numerator = -a.numerator;
    f.denominator = a.denominator;
    f.accuracy = a.accuracy;
    return f;
}

fract abs_fract(fract a) {
    fract f;
    f.numerator = abs(a.numerator);
    f.denominator = abs(a.denominator);
    f.accuracy = a.accuracy;
    return f;
}

double fract_value(fract a) {
    return (double) a.numerator / (double) a.denominator;
}