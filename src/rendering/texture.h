#ifndef _PHI_TEXTURE_H_
#define _PHI_TEXTURE_H_

#include <core/globals.h>
#include <core/resource.h>

#include "rendering.h"

#include <string>
#include <vector>

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

namespace phi
{
    struct texture
    {
    private:
        static texture* _defaultAlbedo;
        static texture* _defaultNormal;
        static texture* _defaultSpecular;
        static texture* _defaultEmissive;

    public:
        static texture* createDefault(byte* data);

    public:
        RENDERING_API static texture* defaultAlbedo();
        RENDERING_API static texture* defaultNormal();
        RENDERING_API static texture* defaultSpecular();
        RENDERING_API static texture* defaultEmissive();

    public:
        GLuint id;
        GLuint64 handle;
        GLuint w;
        GLuint h;
        GLuint type;
        GLint internalFormat;
        GLenum dataFormat;
        GLenum dataType;
        byte* data;

    public:
        RENDERING_API texture::texture(
            uint w, 
            uint h, 
            GLuint type = GL_TEXTURE_2D,
            GLint internalFormat = GL_RGBA8,
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
#endif