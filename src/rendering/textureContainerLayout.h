#pragma once
#include <phi.h>

namespace phi
{
    struct textureContainerLayout
    {
        GLsizei w;
        GLsizei h;
        GLsizei levels;
        GLenum internalFormat;
        GLenum dataFormat;
        GLenum dataType;
        GLint wrapMode;
        GLint minFilter;
        GLint magFilter;

        textureContainerLayout(
            GLsizei w = 0,
            GLsizei h = 0,
            GLsizei levels = 0,
            GLenum internalFormat = GL_RGBA8,
            GLenum dataFormat = GL_BGRA,
            GLenum dataType = GL_UNSIGNED_BYTE,
            GLint wrapMode = GL_REPEAT,
            GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
            GLint magFilter = GL_LINEAR) :
            w(w),
            h(h),
            levels(levels),
            internalFormat(internalFormat),
            dataFormat(dataFormat),
            dataType(dataType),
            wrapMode(wrapMode),
            minFilter(minFilter),
            magFilter(magFilter)
        {
        }
    };
}