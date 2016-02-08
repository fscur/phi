#ifndef _PHI_TEXTURE_H_
#define _PHI_TEXTURE_H_

#include <phi/core/globals.h>
#include <phi/core/size.h>
#include <phi/core/resource.h>

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
        GLuint type;
        byte* data;
        GLenum dataType;
        GLenum dataFormat;
        GLint internalFormat;
        GLuint w;
        GLuint h;

    public:
        RENDERING_API texture(uint w, uint h, GLint internalFormat) :
            w(w), 
            h(h), 
            internalFormat(internalFormat), 
            dataFormat(GL_RGBA), 
            dataType(GL_UNSIGNED_BYTE),
            data(nullptr),
            _isLoadedOnGpu(false),
            type(GL_TEXTURE_2D)
        {
        }

        RENDERING_API texture::texture(uint w, uint h, GLint internalFormat, GLenum dataFormat, GLenum dataType, byte* data) :
            w(w),
            h(h),
            internalFormat(internalFormat),
            dataFormat(dataFormat),
            dataType(dataType),
            data(data),
            _isLoadedOnGpu(false),
            type(GL_TEXTURE_2D)
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