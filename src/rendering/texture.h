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
    class texture
    {
    private:
        static texture* _defaultAlbedo;
        static texture* _defaultNormal;
        static texture* _defaultSpecular;
        static texture* _defaultEmissive;
        static GLuint64 _defaultAlbedoHandle;
        static GLuint64 _defaultNormalHandle;
        static GLuint64 _defaultSpecularHandle;
        static GLuint64 _defaultEmissiveHandle;

    public:
        static texture* createDefault(byte* data);

    public:
        RENDERING_API static texture* defaultAlbedo();
        RENDERING_API static texture* defaultNormal();
        RENDERING_API static texture* defaultSpecular();
        RENDERING_API static texture* defaultEmissive();

    private:
        bool _isLoadedOnGpu;

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
        bool bindless;

    public:
        RENDERING_API texture::texture(
            uint w, 
            uint h, 
            GLuint type = GL_TEXTURE_2D,
            GLint internalFormat = 0, 
            GLenum dataFormat = GL_RGBA,
            GLenum dataType = GL_UNSIGNED_BYTE,
            byte* data = nullptr,
            bool bindless = false) :
            w(w),
            h(h),
            type(type),
            internalFormat(internalFormat),
            dataFormat(dataFormat),
            dataType(dataType),
            data(data),
            bindless(bindless),
            _isLoadedOnGpu(false)
        {
        }

        RENDERING_API ~texture() {}

        RENDERING_API void bind(GLuint level = 0);
        RENDERING_API void setParam(GLenum name, GLint value);
        RENDERING_API void loadOnGpu();
        RENDERING_API void releaseFromGpu();
    };
}
#endif