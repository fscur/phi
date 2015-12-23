#ifndef _PHI_FRAME_BUFFER_H_
#define _PHI_FRAME_BUFFER_H_

#include <phi\core\globals.h>
#include <phi\core\color.h>
#include <phi\core\size.h>

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
        std::map<std::string, renderTarget*>* _renderTargets; //TODO: rendertargets should be value ?

    public:
        RENDERING_API frameBuffer(std::string name, color backColor);
        RENDERING_API virtual ~frameBuffer();

        RENDERING_API GLuint getId() const { return _id; }
        RENDERING_API sizef getSize() const { return _size; }
        RENDERING_API color getClearColor() const { return _clearColor; }
        RENDERING_API bool getIsBound() const { return _isBound; }

        RENDERING_API inline void setSize(sizef value) { _size = value; };
        RENDERING_API inline inline void setClearColor(color value);

        RENDERING_API virtual void init();

        RENDERING_API bool isComplete(const GLenum& target = GL_FRAMEBUFFER);

        RENDERING_API inline void bind();
        RENDERING_API inline void bindForDrawing();
        RENDERING_API inline void bindForDrawing(renderTarget* renderTarget);
        RENDERING_API inline void bindForDrawing(renderTarget* cubeMapRenderTarget, GLuint cubeMapFace);//TODO: reference ?
        RENDERING_API inline void bindForReading();
        RENDERING_API inline void bindForReading(renderTarget* cubeMapRenderTarget, GLuint cubeMapFace);
        RENDERING_API inline void unbind();

        RENDERING_API inline void enable(GLenum value);

        RENDERING_API inline  void setViewport(GLuint x, GLuint y, sizef size);
        RENDERING_API inline virtual void clear();

        RENDERING_API inline void blit(std::string renderTargetName, GLuint x, GLuint y, GLsizei width, GLsizei height, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);
        RENDERING_API inline void blita(GLuint x, GLuint y, GLsizei width, GLsizei height, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);
        RENDERING_API bool addRenderTarget(renderTarget* renderTarget);
        RENDERING_API inline renderTarget* getRenderTarget(std::string name);

        RENDERING_API renderTarget* newRenderTarget(
            std::string name,
            texture* texture,
            GLenum _target = GL_DRAW_FRAMEBUFFER,
            GLenum _attachment = GL_COLOR_ATTACHMENT0,
            GLenum _texTarget = GL_TEXTURE_2D,
            GLuint _level = 0);

        RENDERING_API inline GLfloat getZBufferValue(vec2 mousePos);
    };
}

#endif