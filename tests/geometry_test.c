#include "minunit.h"

#include "defs.h"
#include "geometry.h"

static Vector a = { 1, 2 };
static Vector b = { -2, 3 };


char *test_vector_sum() {
    Vector result = G_Sum(a, b);
    Vector expected = { -1, 5 };

    mu_assert(VEQ(result, expected), "Sum not equal");

    return NULL;
}


char *test_vector_sub() {
    Vector result = G_Sub(a, b);
    Vector expected = { 3, -1 };

    mu_assert(VEQ(result, expected), "Sub not equal");

    return NULL;
}


char *test_dot_product() {
    mu_assert(G_Dot(a, b) == 4, "Dot product not equal");

    return NULL;
}


char *test_cross_product() {
    double result = G_Cross(a, b);
    double expected = 7;

    mu_assert(ARECLOSE(result, expected), "Cross product not equal");

    return NULL;
}



char *all_tests() {
    mu_init();

    mu_run_test(test_vector_sum);
    mu_run_test(test_vector_sub);
    mu_run_test(test_dot_product);
    mu_run_test(test_cross_product);

    return NULL;
}

RUN_TESTS(all_tests);
