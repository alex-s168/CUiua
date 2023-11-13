//
// Created by alex on 10/14/23.
//

#include "utils.h"
#include "main.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <string.h>
#endif

iarr iarr_join(iarr a, iarr b) {
    iarr c;
    c.len = a.len + b.len;
    c.data = malloc(sizeof(int) * c.len);
    if (c.data == NULL) {
        rerror("Out of memory");
    }
    for (int i = 0; i < a.len; i++) {
        c.data[i] = a.data[i];
    }
    for (int i = 0; i < b.len; i++) {
        c.data[i + a.len] = b.data[i];
    }
    return c;
}

void iarr_init(iarr *a, size_t len) {
    a->data = malloc(sizeof(int) * len);
    if (a->data == NULL) {
        rerror("Out of memory");
    }
    a->len = len;
}

void iarr_free(iarr a) {
    free(a.data);
}

void iarr_append(iarr *a, int n) {
    a->len++;
    a->data = realloc(a->data, sizeof(int) * a->len);
    if (a->data == NULL) {
        rerror("Out of memory");
    }
    a->data[a->len - 1] = n;
}

bool iarr_contains(iarr a, int n) {
    for (int i = 0; i < a.len; i++) {
        if (a.data[i] == n) {
            return true;
        }
    }
    return false;
}

// drop the first element
void iarr_drop(iarr *a) {
    for (int i = 0; i < a->len - 1; i++) {
        a->data[i] = a->data[i + 1];
    }
    a->len--;
}

iarr iarr_clone(iarr a) {
    return (iarr) {
        .len = a.len,
        .data = memcpy(malloc(sizeof(int) * a.len), a.data, sizeof(int) * a.len)
    };
}


oIArr oIArrFrom(iarr a) {
    return (oIArr) {
            .len = a.len,
            .data_skip = calloc(sizeof(char), a.len),
            .data = memcpy(malloc(sizeof(int) * a.len), a.data, sizeof(int) * a.len)
    };
}

void oIArrFree(oIArr a) {
    free(a.data_skip);
    free(a.data);
}

iarr oIArrComplete(oIArr a) {
    iarr arr;
    iarr_init(&arr, 0);
    for (size_t i = 0; i < a.len; i ++) {
        if (a.data_skip[i]) {
            continue;
        }
        iarr_append(&arr, a.data[i]);
    }
    return arr;
}

void oIArrRemove(oIArr *a, int n) {
    a->data_skip[n] = 1;
}