//
// Created by alex on 10/14/23.
//

#include "main.h"

#ifdef NOSTD
#include "std.h"
#else
#include <stdlib.h>
#include <string.h>
#include <math.h>
#endif

elem *new_elem(elem_type type) {
    elem *e = malloc(sizeof(elem));
    if (e == NULL) {
        rerror("Out of memory!");
    }
    e->type = type;
    e->f_bool = false;
    e->f_char = false;
    e->is_alloc = true;
    return e;
}

// assumes that every array uses one single memory allocation for all the elements and itself.
// This allocation is starting at array.data
void free_elem(elem *e) {
    if (e == NULL) {
        return;
    }
    if (e->type == BOXED) {
        free_elem(e->data.boxed);
    }
    if (!e->is_alloc) {
        return;
    }
    freex(e);
}

bool elems_equal(elem *a, elem *b) {
    if (a->type != b->type) {
        return false;
    }
    switch (a->type) {
        case BOXED:
            return elems_equal(a->data.boxed, b->data.boxed);
        case TYPE:
            return a->data.type == b->data.type;
        case NUMBER:
            if (isnan(a->data.number) && isnan(b->data.number)) {
                return true;
            }
            return a->data.number == b->data.number;
        case ARRAY:
            if (a->data.array.len != b->data.array.len) {
                return false;
            }
            for (size_t i = 0; i < a->data.array.len; i++) {
                if (!elems_equal(a->data.array.data[i], b->data.array.data[i])) {
                    return false;
                }
            }
            return true;
        case FUNPTR:
            return a->data.ptr == b->data.ptr;
        default:
            return false;
    }
}

// deep-clones an element into a memory location
void eclone_into(elem *dest, elem *src) {
    dest->type = src->type;
    switch (src->type) {
        case FRACTION: {
            dest->data.fraction = src->data.fraction;
        }
        break;

        case BOXED: {
            dest->data.boxed = eclone(src->data.boxed);
        }
        break;

        case NUMBER: {
            dest->data.number = src->data.number;
            dest->f_bool = src->f_bool;
            dest->f_char = src->f_char;
        }
        break;

        case ARRAY: {
            size_t len = src->data.array.len;
            dest->data.array.len = len;

            size_t off = sizeof(elem *) * len;
            void *data = malloc(off + sizeof(elem) * len);
            if (data == NULL) {
                rerror("Out of memory!");
            }
            dest->data.array.data = (elem **) data;
            elem *elems = (elem *) (data + off);
            for (size_t i = 0; i < len; i++) {
                elem *e = &elems[i];
                eclone_into(e, src->data.array.data[i]);
                dest->data.array.data[i] = e;
            }
            add_for_cleanup(data);
        }
        break;

        case FUNPTR: {
            dest->data.ptr = src->data.ptr;
        }
        break;

        default: {
            rerror("Unknown element type %d", src->type);
        }
        break;
    }
}

// deep-clones an element
elem *eclone(elem *e) {
    elem *clone = malloc(sizeof(elem));
    clone->is_alloc = true;
    eclone_into(clone, e);

    unused(e);

    return clone;
}

// copies a element into another one
void copy_elem(elem *dest, elem *src) {
    memcpy(dest, src, sizeof(elem));
}