#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include <string.h>
#include "glad.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#define WINDOW_WIDTH 600
#define WINDOW_WIDTH 600

GLuint newArrayBuffer() {
    GLuint ret = 0;
    glGenBuffers(1, &ret);
    return ret;
}

GLuint newVertexArray() {
    GLuint ret = 0;
    glGenVertexArrays(1, &ret);
    return ret;
}

struct vertline {
    float x;
    GLuint vao;
    GLuint vbo;
};

void bufferDynamicDraw(float* vertices, size_t verticesCount) {
    glBufferData(
        GL_ARRAY_BUFFER,
        verticesCount * sizeof(vertices),
        vertices,
        GL_DYNAMIC_DRAW
    );
}

float* vertlineData(struct vertline instance) {
    const size_t memsize = sizeof(float) * 6;
    float vertices[2][3] = {
        {instance.x, 1, 0},
        {instance.x, -1, 0},
    };

    float *ret = malloc(memsize);
    memcpy(ret, (float*)vertices, memsize);
    return ret;
}

void vertlineUpdate(struct vertline instance) {
    float* vertices = vertlineData(instance);
    glBindBuffer(GL_ARRAY_BUFFER, instance.vbo);
    bufferDynamicDraw(vertices, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glRaise("fail updating vertical line data", glGetError());
    free(vertices);
}

void vertlineBind(struct vertline instance) {
    glBindVertexArray(instance.vao);
}

void vertlineUnbind(struct vertline instance) {
    glBindVertexArray(0);
}

void vertexAttribPointer() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        0
    );
}

struct vertline makeLine() {
    struct vertline ret = {
        .x = 0,
        .vao = newVertexArray(),
        .vbo = newArrayBuffer(),
    };

    float* vertices = vertlineData(ret);
    glBindVertexArray(ret.vao);
    glBindBuffer(GL_ARRAY_BUFFER, ret.vbo);
    bufferDynamicDraw((float*)vertices, 6);
    vertexAttribPointer();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glRaise("fail making a line", glGetError());
    return ret;
}


GLuint createShader(const char* filepath, GLenum shaderType) {
    const int size = 4096;
    FILE *vertFD = fopen(filepath, "r");
    if (vertFD == 0)
        glibcRaise();

    char *buf = malloc(size);
    memset(buf, 0, size);
    int readDataSize = fread(buf, size, 1, vertFD);
    int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, (const GLchar**)&buf, 0);
    glRaise("fail to load shader source", glGetError());
    glCompileShader(shaderID);
    glRaise("fail to compile shader source", glGetError());

    GLint status = 1;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    char prefix[256];
    sprintf(prefix, "fail to get compile status shader %s", filepath);
    glRaise(prefix, glGetError());
    if (status == 0) {
        GLint datalen = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &datalen);
        char *data = malloc(datalen);
        glGetShaderInfoLog(shaderID, datalen, 0, data);
        printf("%s shader compiling logs: %s", filepath, data);
        free(data);
        exit(-1);
    }

    free(buf);
    return shaderID;
}

GLuint makeProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    glDetachShader(programID, vertexShader);
    glDetachShader(programID, fragmentShader);
    glRaise("fail to make a program", glGetError());

    GLint status = 1;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == 0) {
        GLint datalen = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &datalen);
        char *data = malloc(datalen);
        glGetProgramInfoLog(programID, datalen, 0, data);
        printf("fail to link a program: %s", data);
        free(data);
        exit(-1);
    }

    return programID;
}

struct mousepos {
    double col;
    double row;
} mousepos;

void on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
    mousepos.col = xpos;
    mousepos.row = ypos;
}

int main()
{
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_WIDTH, "hello line", 0, 0);
    glfwRaise();
    glfwMakeContextCurrent(window);
    glfwRaise();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("fail to load GLAD\n");
        return 1;
    }

    struct vertline line = makeLine();
    GLuint vertexShaderID = createShader("./point.vert", GL_VERTEX_SHADER);
    GLuint fragmentShaderID = createShader("./point.frag", GL_FRAGMENT_SHADER);
    GLuint lineProgramID = makeProgram(vertexShaderID, fragmentShaderID);
    glViewport(0, 0, 600, 600);
    glClearColor(.3, .3, .5, 1.);
    if (!glfwSetCursorPosCallback(window, on_mouse_move))
        glfwRaise("fail to set mouse move callback");

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        line.x = (mousepos.col / WINDOW_WIDTH) * 2. - 1.;
        vertlineUpdate(line);
        glUseProgram(lineProgramID);
        glRaise("fail to use line shader", glGetError());
        vertlineBind(line);
        glRaise("fail to bind line data", glGetError());
        glDrawArrays(GL_LINES, 0, 2);
        vertlineUnbind(line);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
