#pragma once

#include <stdio.h>

#include <epicsExit.h>

#define fer_epics_panic(...) \
    do { \
        printf("Ferrite panicked at %s:%d", __FILE__, __LINE__); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        epicsExit(2); \
    } while (0)

#define fer_epics_assert(expr) \
    do { \
        if (__builtin_expect(!(expr), 0)) { \
            fer_epics_panic("Assertion failed: " #expr " is false"); \
        } \
    } while (0)

#define fer_epics_unreachable() \
    do { \
        if (__builtin_expect(1, 0)) { \
            fer_epics_panic("Unreachable code reached"); \
        } \
    } while (0)
