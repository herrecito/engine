#include <stdlib.h>

#include "minunit.h"

#include "collision.h"
#include "defs.h"
#include "geometry.h"
#include "map.h"

int test_check_point() {
    int CheckPoint(Vector p, Mobile mob, double *distance, double *t0);

    Vector point = {2, 0};
    Mobile mob = {
        .pos = {0, 0},
        .vel = {2, 0},
        .radius = 1
    };

    double distance, t0;
    mu_assert(CheckPoint(point, mob, &distance, &t0), "Detects the collision");

    mu_assert(EQ(distance, 2), "Gets the distance right");
    mu_assert(EQ(t0, 0.5), "Gets the time right");

    return 0;
}


int test_check_collision() {
    Map m = {
        .name = "Test Map",
        .spawn = {0, 0},
        .numwalls = 1,
        .walls = malloc(sizeof(Wall))
    };

    m.walls[0] = (Wall){
        .seg = {
            .start = {1, -1},
            .end   = {1,  1}
        }
    };

    Mobile mob = {
        .pos = {0, 0},
        .vel = {2, 0},
        .radius = 1
    };

    Vector collision;
    Vector expected_collision = {1, 0};

    double t0;
    double expected_t0 = 0;

    mu_assert(Co_CheckCollision(&m, mob, &collision, &t0),
            "Detects the collision");

    mu_assert(VEQ(collision, expected_collision), "Gets the collision right");
    mu_assert(EQ(t0, expected_t0), "Gets the time right");

    return 0;
}



int all_tests() {
    mu_run_test(test_check_point);
    mu_run_test(test_check_collision);

    return 0;
}

RUN_TESTS(all_tests);
