//
// Created by alex on 10/19/23.
//

#ifndef CUIUA_CONFIG_H
#define CUIUA_CONFIG_H

/* prints debug information (like for example when the stack got reallocated) */
// #define STACK_DEBUG

/* reduces system time by not freeing elements during calculations.
 * This will increase memory usage.
 */
// #define FAST_FREE

/* print cleanup debug information */
// #define CLEANUP_DEBUG

/* do not cleanup elements */
// #define NO_CLEANUP

/* instead of freeing each element on the stack, add it to cleanup list */
// #define STACK_CLEANUP

#endif //CUIUA_CONFIG_H
