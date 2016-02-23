#pragma once

#include <core/globals.h>
#include <core/resource.h>

#include "rendering.h"

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

namespace phi
{
    struct texture
    {
    public:
        GLuint id;
        GLuint64 handle;
        GLuint w;
        GLuint h;
        GLenum type;
        GLenum internalFormat;
        GLenum dataFormat;
        GLenum dataType;
        byte* data;

    public:
        RENDERING_API texture::texture(
            uint w, 
            uint h, 
            GLenum type = GL_TEXTURE_2D,
            GLenum internalFormat = GL_RGBA8,
            GLenum dataFormat = GL_BGRA,
            GLenum dataType = GL_UNSIGNED_BYTE,
            byte* data = nullptr) :
            w(w),
            h(h),
            type(type),
            internalFormat(internalFormat),
            dataFormat(dataFormat),
            dataType(dataType),
            data(data)
        {
        }

        RENDERING_API ~texture() {}
    };
}