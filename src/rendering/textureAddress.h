#pragma once
#include <phi.h>
#include <core\rectangle.h>

namespace phi
{
    struct textureAddress
    {
        GLuint arrayId;
        GLint index;
        GLint unit;
        GLfloat page;
        rectangle<uint> rect;
    };
}