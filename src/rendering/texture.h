#pragma once
#include <phi.h>
#include <core\resource.h>

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
        GLint wrapMode;
        GLint minFilter;
        GLint magFilter;
        bool generateMipmaps;

    public:
        texture(
            uint w, 
            uint h, 
            GLenum type = GL_TEXTURE_2D,
            GLenum internalFormat = GL_RGBA8,
            GLenum dataFormat = GL_BGRA,
            GLenum dataType = GL_UNSIGNED_BYTE,
            byte* data = nullptr,
            GLint wrapMode = GL_REPEAT,
            GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
            GLint magFilter = GL_LINEAR,
            bool generateMipmaps = true) :
            w(w),
            h(h),
            type(type),
            internalFormat(internalFormat),
            dataFormat(dataFormat),
            dataType(dataType),
            data(data),
            wrapMode(wrapMode),
            minFilter(minFilter),
            magFilter(magFilter),
            generateMipmaps(generateMipmaps)
        {
        }

        ~texture() {}
    };
}