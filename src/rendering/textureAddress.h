#pragma once
#include <phi.h>
#include <core\rectangle.h>

namespace phi
{
    struct textureAddress
    {
        GLuint containerId;
        GLint unit;
        GLfloat page;
        rectangle<uint> rect;
    };
}