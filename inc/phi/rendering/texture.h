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
        GLuint _id;
        GLuint _textureType;
        byte* _data;
        GLenum _dataType;
        GLenum _dataFormat;
        GLint _internalFormat;
        uint _w;
        uint _h;
        bool _loadedOnGpu;

    public:
        RENDERING_API texture::texture(uint w, uint h, GLint internalFormat);
        RENDERING_API texture::texture(uint w, uint h, GLint internalFormat, GLenum dataFormat, GLenum type, byte* data);

    private:
        static texture* createDefault(byte* data);

    public:
        RENDERING_API static texture* defaultDiffuse;
        RENDERING_API static texture* defaultNormal;
        RENDERING_API static texture* defaultSpecular;
        RENDERING_API static texture* defaultEmissive;

    public:
        RENDERING_API ~texture();

        RENDERING_API uint getId() const { return _id; }
        RENDERING_API uint getWidth() const { return _w; }
        RENDERING_API uint getHeight() const { return _h; }
        RENDERING_API uint getTextureType() const { return _textureType; }

        RENDERING_API void bind(GLuint level = 0);
        RENDERING_API void setParam(GLenum name, GLint value);
        RENDERING_API void loadOnGpu();
        RENDERING_API void releaseFromGpu();
    };
}
#endif