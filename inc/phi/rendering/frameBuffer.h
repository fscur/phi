#ifndef _PHI_FRAME_BUFFER_H_
#define _PHI_FRAME_BUFFER_H_

#include <phi/core/globals.h>
#include <phi/core/color.h>
#include <phi/core/size.h>

#include "rendering.h"
#include "renderTarget.h"

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <glm/glm.hpp>
#include <map>

namespace phi
{
    class frameBuffer
    {
    protected:
        std::string _name;
        bool _isInitialized;
        GLuint _id;
        GLuint _x;
        GLuint _y;
        sizef _size;
        bool _isBound;
        color _clearColor;
        std::map<std::string, renderTarget*>* _renderTargets;

    public:
        RENDERING_API frameBuffer(std::string name, sizef size, color backColor);
        RENDERING_API virtual ~frameBuffer();

        RENDERING_API GLuint getId() const { return _id; }
        RENDERING_API sizef getSize() const { return _size; }
        RENDERING_API color getClearColor() const { return _clearColor; }
        RENDERING_API bool getIsBound() const { return _isBound; }

        RENDERING_API void setSize(sizef value);
        RENDERING_API void setClearColor(color value);

        RENDERING_API virtual void init();

        RENDERING_API bool isComplete(GLenum target = GL_FRAMEBUFFER);

        RENDERING_API void bind();
        RENDERING_API void bindForDrawing();
        RENDERING_API void bindForDrawing(renderTarget* renderTarget);
        RENDERING_API void bindForDrawing(renderTarget* cubeMapRenderTarget, GLuint cubeMapFace);
        RENDERING_API void bindForReading();
        RENDERING_API void bindForReading(renderTarget* cubeMapRenderTarget, GLuint cubeMapFace);
        RENDERING_API void unbind();

        RENDERING_API void enable(GLenum value);

        RENDERING_API void setViewport(GLuint x, GLuint y, sizef size);
        RENDERING_API virtual void clear();

        RENDERING_API void blit(std::string renderTargetName, GLuint x, GLuint y, GLsizei width, GLsizei height, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);
        RENDERING_API void blita(GLuint x, GLuint y, GLsizei width, GLsizei height, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);
        RENDERING_API bool addRenderTarget(renderTarget* renderTarget);
        RENDERING_API renderTarget* getRenderTarget(std::string name);

        RENDERING_API renderTarget* newRenderTarget(
            std::string name,
            texture* texture,
            GLenum _target = GL_DRAW_FRAMEBUFFER,
            GLenum _attachment = GL_COLOR_ATTACHMENT0,
            GLenum _texTarget = GL_TEXTURE_2D,
            GLuint _level = 0);

        RENDERING_API GLfloat getZBufferValue(vec2 mousePos);
    };
}

#endif