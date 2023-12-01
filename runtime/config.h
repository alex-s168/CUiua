//
// Created by alex on 10/19/23.
//

#ifndef CUIUA_CONFIG_H
#define CUIUA_CONFIG_H

/* =============================================== */
/*              CONFIGURATION OPTIONS              */
/* =============================================== */

/* prints debug information (like for example when the stack got reallocated) */
// #define STACK_DEBUG

/* reduces system time by not freeing elements during calculations.
 * This will increase memory usage.
 */
#define FAST_FREE

/*
 * Do not do memory management at all. (useful for short-lived programs)
 */
#define NO_MM

/* prints debug information when an element is freed via freexe */
// #define FREEXE_LOG

/* print cleanup debug information */
// #define CLEANUP_DEBUG

/* do not cleanup elements */
#define NO_CLEANUP

/* instead of freeing each element on the stack, add it to cleanup list */
// #define STACK_CLEANUP

/* =============================================== */

#ifdef NO_MM
#define NO_CLEANUP
#undef STACK_CLEANUP
#endif

#endif //CUIUA_CONFIG_H
