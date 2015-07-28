#undef NDEBUG

#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#define mu_init() char *message = NULL

#define mu_assert(test, message) \
    if (!(test)) {\
        log_err(message);\
        return message;\
    }

#define mu_run_test(test) \
        message = test();\
        tests_run++;\
        if (message) return message;

#define RUN_TESTS(name) \
    int main(int argc, char **argv) {\
        printf("RUNNING: %s\n", argv[0]);\
        char *result = name();\
        if (result != 0) {\
            printf("TEST FAILED: %s\n", result);\
        } else {\
            printf("ALL TEST PASSED\n");\
        }\
        printf("Tests run: %d\n", tests_run);\
        exit(result != 0);\
    }

int tests_run;

#endif
