//
// Created by alex on 10/14/23.
//

#ifndef CUIUA_UTILS_H
#define CUIUA_UTILS_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    int *data;
    size_t len;
} iarr;

iarr iarr_join(iarr a, iarr b);

void iarr_init(iarr *a, size_t len);

void iarr_free(iarr a);

void iarr_append(iarr *a, int n);

bool iarr_contains(iarr a, int n);

// drop the first element
void iarr_drop(iarr *a);

#endif //CUIUA_UTILS_H
