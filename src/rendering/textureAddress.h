#pragma once

#include <GL\glew.h>

namespace phi
{
    struct textureAddress
    {
        GLuint containerId;
        GLint unit;
        GLfloat page;
    };
}