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
        bool _isLoadedOnGpu;

    public:
        RENDERING_API texture::texture(uint w, uint h, GLint internalFormat);
        RENDERING_API texture::texture(uint w, uint h, GLint internalFormat, GLenum dataFormat, GLenum type, byte* data);

    private:
        static texture* defaultDiffuse;
        static texture* defaultNormal;
        static texture* defaultSpecular;
        static texture* defaultEmissive;

    private:
        static texture* createDefault(byte* data);

    public:
        RENDERING_API static texture* getDefaultDiffuse();
        RENDERING_API static texture* getDefaultNormal();
        RENDERING_API static texture* getDefaultSpecular();
        RENDERING_API static texture* getDefaultEmissive();

    public:
        RENDERING_API ~texture();

        RENDERING_API uint getId() const { return _id; }
        RENDERING_API uint getWidth() const { return _w; }
        RENDERING_API uint getHeight() const { return _h; }
        RENDERING_API uint getTextureType() const { return _textureType; }
        RENDERING_API byte* getData() const { return _data; }
        RENDERING_API GLenum getDataFormat() const { return _dataFormat; }
        RENDERING_API GLenum getDataType() const { return _dataType; }

        RENDERING_API void bind(GLuint level = 0);
        RENDERING_API void setParam(GLenum name, GLint value);
        RENDERING_API void loadOnGpu();
        RENDERING_API void releaseFromGpu();
    };
}
#endif