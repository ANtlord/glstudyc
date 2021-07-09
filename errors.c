#include "errors.h"

void glfwRaise(char* prefix)
{
    const char *msg = 0;
    if (glfwGetError(&msg) == GLFW_NO_ERROR)
        return;

    glfwTerminate();
    printf("%s: %s\n", prefix, msg);
    exit(-1);
}

void glibcRaise()
{
    if (0 == errno)
        return;

    printf("glibc error: %s\n", strerror(errno));
    glfwTerminate();
    exit(-1);
}

void glRaise(char* prefix, GLenum err)
{
    if (GL_NO_ERROR == err)
        return;

    printf("%s: %s (%d)\n", prefix, glGetString(err), err);
    glfwTerminate();
    exit(-1);
}
