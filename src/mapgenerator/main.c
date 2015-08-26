#include <math.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "lib/buffer.h"
#include "lib/color.h"
#include "lib/draw.h"
#include "lib/system.h"
#include "lib/geometry.h"
#include "lib/map.h"

#define WIDTH 640
#define HEIGHT 400

const Box SCREEN = { 0, HEIGHT-1, 0, WIDTH-1 };

Box rooms[30];

void DrawMap(Map *map, Buffer *buf) {
    for (int i = 0; i < map->numwalls; i++) {
        Segment seg = map->walls[i].seg;

        Segment clipped;
        G_ClipSegment(seg, SCREEN, &clipped);

        D_DrawSegment(buf, clipped, RED);
    }
}

// Returns a random number between [0, 1]
double drand() {
    return (double)rand() / (double)RAND_MAX;
}


Vector RandomPointInCircle(double radius) {
    double t = 2 * M_PI * drand();
    double u = 2 * drand();
    double r = u > 1 ? 2 - u : u;
    Vector point = {
        .x = WIDTH/2 + radius * r * cos(t),
        .y = HEIGHT/2 + radius * r * sin(t)
    };

    return point;
}


Box GenerateRandomRoom() {
    Vector center = RandomPointInCircle(100);
    double w = 10 + drand() * 25;
    double h = 10 + drand() * 25;

    Box room = {
        .top = center.y - h,
        .bottom = center.y + h,
        .left = center.x - w,
        .right = center.x + w
    };

    return room;
}


void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q) {
        exit(EXIT_SUCCESS);
    }
}


int main() {
    GLFWwindow *window = S_InitWindow(WIDTH, HEIGHT);
    glfwSetKeyCallback(window, KeyCallback);
    Buffer *buffer = B_CreateBuffer(WIDTH, HEIGHT);

    Box rooms[50];
    for (int i = 0; i < 25; i++) {
        rooms[i] = GenerateRandomRoom();
        D_DrawBox(buffer, rooms[i], RED);
    }

    while (1) {
        S_Blit(window, buffer);
        glfwPollEvents();
    }

    B_DeleteBuffer(buffer);
    glfwTerminate();
}
