#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "glad.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

void glRaise(char*, GLenum);
void glibcRaise();
void glfwRaise();
#endif // ERRORS_H
