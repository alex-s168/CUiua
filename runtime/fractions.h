//
// Created by alex on 10/17/23.
//

#ifndef CUIUA_FRACTIONS_H
#define CUIUA_FRACTIONS_H

// fraction
typedef struct {
    int numerator;
    int denominator;

    // when converting from a double, this is the number of digits after the decimal point
    int accuracy;
} fract;

fract to_fraction(double d);

fract add_fract(fract a, fract b);

fract add_fract_d(fract a, double b);

fract sub_fract(fract a, fract b);
fract sub_fract_d0(double a, fract b);
fract sub_fract_d1(fract a, double b);

fract mul_fract(fract a, fract b);
fract mul_fract_d(fract a, double b);

fract div_fract(fract a, fract b);
fract div_fract_d0(double a, fract b);
fract div_fract_d1(fract a, double b);

fract pow_fract(fract a, fract b);
fract pow_fract_d0(double a, fract b);
fract pow_fract_d1(fract a, double b);

fract neg_fract(fract a);

fract abs_fract(fract a);

double fract_value(fract a);

fract fract_shorten(fract a);

#endif //CUIUA_FRACTIONS_H
