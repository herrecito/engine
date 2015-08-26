#ifndef _ENGINE_
#define _ENGINE_

#include <stdint.h>

#include <GLFW/glfw3.h>

#include "buffer.h"

GLFWwindow *S_InitWindow(int width, int height);
uint32_t S_GetTime();
uint32_t S_Blit(GLFWwindow *window, Buffer *buf);

#endif
