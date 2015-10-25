#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "buffer.h"
#include "system.h"


static char *ReadFile(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Coudn't read file: '%s'\n", path);
    }

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(fsize + 1);
    fread(buf, fsize, 1, f);
    buf[fsize] = '\0';

    fclose(f);

    return buf;
}


static GLuint CompileShaderProgram() {
    char *vertexSource = ReadFile("vertex.glsl");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar **)&vertexSource, NULL);
    glCompileShader(vertexShader);
    free(vertexSource);

    char *fragmentSource = ReadFile("frag.glsl");
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar **)&fragmentSource, NULL);
    glCompileShader(fragmentShader);
    free(fragmentSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


static GLuint BuildQuad(GLuint shaderProgram) {
    GLfloat vertices[] = {
        // Vertex coord  Texture coord
        1,  1,           1, 0,
        -1,  1,          0, 0,
        1, -1,           1, 1,
        -1, -1,          0, 1
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex attributes
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            0);
    glEnableVertexAttribArray(posAttrib);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}


static GLuint BuildTexture(int width, int height) {
    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}


static void InitOpenGL(int width, int height) {
    GLuint shaderProgram = CompileShaderProgram();
    GLuint quad_vao = BuildQuad(shaderProgram);
    GLuint tex = BuildTexture(width, height);

    glUseProgram(shaderProgram);
    glBindVertexArray(quad_vao);
    glBindTexture(GL_TEXTURE_2D, tex);
}


GLFWwindow *S_InitWindow(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window and OpenGL context
    GLFWwindow *window = glfwCreateWindow(width, height, "engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Disable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    InitOpenGL(width, height);

    return window;
}


uint32_t S_GetTime() {
    return 1000 * glfwGetTime();
}


uint32_t S_Blit(GLFWwindow *window, Buffer *buf) {
    uint32_t start = S_GetTime();

    glTexSubImage2D(
            GL_TEXTURE_2D,              // target
            0,                          // level of detail
            0, 0,                       // xoffset, yoffset
            buf->width, buf->height,    // width, height
            GL_RGBA, GL_UNSIGNED_BYTE,  // format, type
            buf->pixels                 // pixels
            );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glfwSwapBuffers(window);

    return S_GetTime() - start;
}
