#ifndef _PHI_TEXTURE_H_
#define _PHI_TEXTURE_H_

#include <phi/core/globals.h>
#include <phi/core/size.h>
#include <phi/core/resource.h>

#include "rendering.h"
#include "SDL_Extensions.h"

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

    protected:
        uint _w;
        uint _h;

    private:
        texture(uint id, uint w, uint h);

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
        RENDERING_API void release();

        RENDERING_API static texture* fromFile(std::string fileName);
        RENDERING_API static texture* create(uint w, uint h, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, GLvoid* data = 0);
        RENDERING_API static texture* createCubeMap(uint w, uint h, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, const std::vector<GLvoid*> data = std::vector<GLvoid*>());
    };
}
#endif