#include "minunit.h"

#include "lib/defs.h"
#include "lib/geometry.h"


int test_vector_sum() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Vector result = G_Sum(a, b);
    Vector expected = { -1, 5 };

    mu_assert(VEQ(result, expected), "Sum not equal");

    return 0;
}


int test_vector_sub() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Vector result = G_Sub(a, b);
    Vector expected = { 3, -1 };

    mu_assert(VEQ(result, expected), "Sub not equal");

    return 0;
}


int test_dot_product() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    mu_assert(G_Dot(a, b) == 4, "Dot product not equal");

    return 0;
}


int test_cross_product() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    double result = G_Cross(a, b);
    double expected = 7;

    mu_assert(EQ(result, expected), "Cross product not equal");

    return 0;
}

int test_parallel() {
    Vector a = {1, 0};
    Vector b = {0.001, 0};

    mu_assert(G_Parallel(a, b), "Works as expected");

    return 0;
}


int test_project() {
    Vector a = {2, 0};
    Vector dir = {1, 0};
    Vector expected = {2, 0};

    mu_assert(VEQ(G_Project(a, dir), expected), "Works for trivial case");

    a = (Vector){1, 0};
    dir = (Vector){3, 0};
    expected = (Vector){1, 0};

    mu_assert(VEQ(G_Project(a, dir), expected), "Works for non unitary directions");

    a = (Vector){-1, 0};
    dir = (Vector){2, 0};
    expected = (Vector){-1, 0};

    mu_assert(VEQ(G_Project(a, dir), expected), "Works for negative vectors");

    return 0;
}


int test_ray_line_intersection() {
    Line ray = { .start = {0, 0}, .dir = {1, 0} };
    Line l = { .start = {2, 1}, .dir = {0, 1} };
    Vector intersection = {2, 0};

    Vector result;
    mu_assert(G_RayLineIntersection(ray, l, &result),
            "Detects the intersection");

    mu_assert(VEQ(intersection, result),
            "Correctly calculates the intersection");


    ray = (Line){ .start = {0, 0}, .dir = {1, 0} };
    l   = (Line){ .start = {0, 1}, .dir = {1, 0} };

    mu_assert(G_RayLineIntersection(ray, l, NULL) == 0,
            "Works as expected when ray and line are parallel");

    return 0;
}


int test_line_line_intersection() {
    Line l1 = { .start = {0, 0}, .dir = {1, 0} };
    Line l2 = { .start = {0, 1}, .dir = {1, 0} };

    mu_assert(G_LineLineIntersection(l1, l2, NULL) == 0,
            "Works as expected when lines are parallel");

    l1 = (Line){ .start = {0, 3}, .dir = {0, -1} };
    l2 = (Line){ .start = {-1, 0}, .dir = {1, 0} };
    Vector expected = {0, 0};

    Vector result;
    mu_assert(G_LineLineIntersection(l1, l2, &result),
            "Detects the intersection");

    mu_assert(VEQ(expected, result),
            "Correctly calculates the intersection");

    return 0;
}


int test_line_point_distance() {
    Line l1 = { .start = {0, 0}, .dir = {1, 0} };
    Vector p = {0, 3};

    double expected = 3;

    mu_assert(EQ(expected, G_LinePointDistance(l1, p)),
            "Correctly calculates the distance to the line");

    return 0;
}


int test_rotate() {
    Vector a = { 1, 2 };
    double angle = M_PI / 2;
    Vector expected = { -2, 1 };

    mu_assert(VEQ(G_Rotate(a, angle), expected), "Rotate not equal");

    return 0;
}


int test_translate_segment() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };

    Segment s = { a, b };
    Vector d = { 1, 0 };
    Segment expected = { { 2, 2 }, { -1, 3} };

    mu_assert(SEGEQ(G_TranslateSegment(s, d), expected),
            "Translate segment not equal");

    return 0;
}


int test_midpoint() {
    Vector a = { 1, 2 };
    Vector b = { -2, 3 };
    Vector expected = { -0.5, 2.5 };

    mu_assert(VEQ(G_Midpoint(a, b), expected), "Midpoint not equal");

    return 0;
}


int test_rotate_segment() {
    Segment s = { {-1, 0}, {1, 0} };
    double angle = -M_PI / 2;
    Segment expected = { {0, 1}, {0, -1} };

    mu_assert(SEGEQ(G_RotateSegment(s, angle), expected),
            "Rotate segment not equal");

    return 0;
}


int test_rotate_segment_around_point() {
    Segment s = { {-1, 0}, {1, 0} };
    double angle = -M_PI / 2;
    Vector point = { 0, 0 };
    Segment expected = { {0, 1}, {0, -1} };

    mu_assert(SEGEQ(G_RotateSegmentAroundPoint(s, angle, point), expected),
            "For (0, 0) behaves as RotateSegment()");

    s = (Segment){ {-1, 1}, {1, 1} };
    angle = -M_PI / 2;
    point = (Vector){0, 1};
    expected = (Segment){ {0, 2}, {0, 0} };

    mu_assert(SEGEQ(G_RotateSegmentAroundPoint(s, angle, point), expected),
            "Works for arbitrary points as well");

    return 0;
}


int test_normal() {
    Line l = { .start = {0, 0}, . dir = {1, 0} };
    Vector expected = {0, 1};

    mu_assert(VEQ(G_Normal(l), expected), "It works as expected");

    return 0;
}


int test_support_line() {
    Segment seg = { {1, 1}, {2, 1} };
    Line expected = { {1, 1}, {1, 0} };

    mu_assert(LINEQ(G_SupportLine(seg), expected), "It works as expected");

    return 0;
}


int test_is_point_on_segment() {
    Segment seg = { {0, 0}, {2, 2} };
    Vector on = {1, 1};

    mu_assert(G_IsPointOnSegment(seg, on), "It works as expected");

    return 0;
}


int all_tests() {
    mu_run_test(test_vector_sum);
    mu_run_test(test_vector_sub);
    mu_run_test(test_dot_product);
    mu_run_test(test_cross_product);
    mu_run_test(test_parallel);
    mu_run_test(test_translate_segment);
    mu_run_test(test_project);
    mu_run_test(test_midpoint);
    mu_run_test(test_rotate);
    mu_run_test(test_rotate_segment);
    mu_run_test(test_rotate_segment_around_point);
    mu_run_test(test_ray_line_intersection);
    mu_run_test(test_line_line_intersection);
    mu_run_test(test_line_point_distance);
    mu_run_test(test_normal);
    mu_run_test(test_support_line);
    mu_run_test(test_is_point_on_segment);

    return 0;
}

RUN_TESTS(all_tests);
