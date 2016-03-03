#pragma once
#include <phi.h>

namespace phi
{
    struct textureAddress
    {
        GLuint containerId;
        GLint unit;
        GLfloat page;
    };
}