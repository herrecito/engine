#include "minunit.h"

#include "defs.h"
#include "geometry.h"


char *test_vector_sum() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Vector result = G_Sum(a, b);
    Vector expected = { -1, 5 };

    mu_assert(VEQ(result, expected), "Sum not equal");

    return NULL;
}


char *test_vector_sub() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Vector result = G_Sub(a, b);
    Vector expected = { 3, -1 };

    mu_assert(VEQ(result, expected), "Sub not equal");

    return NULL;
}


char *test_dot_product() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    mu_assert(G_Dot(a, b) == 4, "Dot product not equal");

    return NULL;
}


char *test_cross_product() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    double result = G_Cross(a, b);
    double expected = 7;

    mu_assert(EQ(result, expected), "Cross product not equal");

    return NULL;
}


char *test_ray_line_intersection() {
    Line ray = { .start = {0, 0}, .dir = {1, 0} };
    Line l = { .start = {2, 1}, .dir = {0, 1} };
    Vector intersection = {2, 0};

    Vector result;
    mu_assert(G_RayLineIntersection(ray, l, &result),
            "Detects the intersection");

    mu_assert(VEQ(intersection, result),
            "Correctly calculates the intersection")

    return NULL;
}


char *test_rotate() {
    Vector a = { 1, 2 };
    double angle = PI / 2;
    Vector expected = { -2, 1 };

    mu_assert(VEQ(G_Rotate(a, angle), expected), "Rotate not equal");

    return NULL;
}


char *test_translate_segment() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Segment s = { a, b };
    Vector d = { 1, 0 };
    Segment expected = { { 2, 2 }, { -1, 3} };

    mu_assert(SEGEQ(G_TranslateSegment(s, d), expected),
            "Translate segment not equal");

    return NULL;
}


char *test_segment_center() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Segment s = { a, b };
    Vector expected = { -0.5, 2.5 };

    mu_assert(VEQ(G_SegmentCenter(s), expected), "Segment center not equal");

    return NULL;
}


char *test_rotate_segment() {
    Segment s = { {-1, 0}, {1, 0} };
    double angle = -PI / 2;
    Segment expected = { {0, 1}, {0, -1} };

    mu_assert(SEGEQ(G_RotateSegment(s, angle), expected),
            "Rotate segment not equal");

    return NULL;
}


char *test_rotate_segment_around_point() {
    Segment s = { {-1, 0}, {1, 0} };
    double angle = -PI / 2;
    Vector point = { 0, 0 };
    Segment expected = { {0, 1}, {0, -1} };

    mu_assert(SEGEQ(G_RotateSegmentAroundPoint(s, angle, point), expected),
            "For (0, 0) behaves as RotateSegment()");

    s = (Segment){ {-1, 1}, {1, 1} };
    angle = -PI / 2;
    point = (Vector){0, 1};
    expected = (Segment){ {0, 2}, {0, 0} };

    mu_assert(SEGEQ(G_RotateSegmentAroundPoint(s, angle, point), expected),
                "Works for arbitrary points as well");

    return NULL;
}


char *all_tests() {
    mu_init();

    mu_run_test(test_vector_sum);
    mu_run_test(test_vector_sub);
    mu_run_test(test_dot_product);
    mu_run_test(test_cross_product);
    mu_run_test(test_translate_segment);
    mu_run_test(test_segment_center);
    mu_run_test(test_rotate);
    mu_run_test(test_rotate_segment);
    mu_run_test(test_rotate_segment_around_point);
    mu_run_test(test_ray_line_intersection);

    return NULL;
}

RUN_TESTS(all_tests);
