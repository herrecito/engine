#undef NDEBUG

#ifndef _minunit_h
#define _minunit_h

#define ANSI_COLOR_RED   "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include <stdio.h>

#define mu_assert(test, message, ...) \
    if (!(test)) {\
        printf(ANSI_COLOR_RED);\
        printf("\tInvalid assertion %s (%s:%d):\n", #test, __FILE__, __LINE__);\
        printf("\t\"" message "\"\n", ##__VA_ARGS__);\
        printf(ANSI_COLOR_RESET);\
        return 1;\
    }

#define mu_run_test(test) \
    if (test()) {\
        failures++;\
        printf(ANSI_COLOR_RED);\
        printf("\t%s failed.\n", #test);\
        printf(ANSI_COLOR_RESET);\
    }\
    ntests++;\

#define RUN_TESTS(tests) \
    int main(int argc, char **argv) {\
        printf("Running %s ...\n", argv[0]);\
        tests();\
        printf("%d Tests, %d failures.\n\n", ntests, failures);\
    }

static int ntests;
static int failures;

#endif
