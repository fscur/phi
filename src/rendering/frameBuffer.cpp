#include "frameBuffer.h"

namespace phi
{
    frameBuffer::frameBuffer(std::string name, sizef size, color clearColor) :
        _name(name),
        _size(size),
        _clearColor(clearColor),
        _x(0),
        _y(0),
        _isBound(false),
        _isInitialized(false),
        _id(0),
        _renderTargets(new std::map<std::string, renderTarget*>())
    {
    }

    frameBuffer::~frameBuffer()
    {
        if (_renderTargets)
        {
            for(auto i = _renderTargets->begin(); i != _renderTargets->end(); ++i)
            {
                safeDelete(i->second);
            }

            safeDelete(_renderTargets);
        }

        glDeleteFramebuffers(1, &_id);
    }

    void frameBuffer::init()
    {
        glGenFramebuffers(1, &_id);
        glBindFramebuffer(GL_FRAMEBUFFER, _id);

        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool frameBuffer::isComplete(const GLenum& target)//TODO: test const reference in opengl functions
    {
        GLenum status = glCheckFramebufferStatus(target);

        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string msg = "FB error, status: " + std::to_string(status);
            log(msg);
            return false;
        }

        return true;
    }

    void frameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _id);
    }

    void frameBuffer::bindForDrawing()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
    }

    void frameBuffer::bindForDrawing(renderTarget* renderTarget)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
        glDrawBuffer(renderTarget->getAttachment());
    }

    void frameBuffer::bindForDrawing(renderTarget* cubeMapRenderTarget, GLuint cubeMapFace)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, cubeMapRenderTarget->getAttachment(), cubeMapFace, cubeMapRenderTarget->getTexture()->id, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapFace, cubeMapRenderTarget->getTexture()->id +1, 0);
        glDrawBuffer(cubeMapRenderTarget->getAttachment());
    }

    void frameBuffer::bindForReading()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
    }

    void frameBuffer::bindForReading(renderTarget* cubeMapRenderTarget, GLuint cubeMapFace)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, cubeMapRenderTarget->getAttachment(), cubeMapFace, cubeMapRenderTarget->getTexture()->id, 0);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapFace, cubeMapRenderTarget->getTexture()->id +1, 0);
        glReadBuffer(cubeMapRenderTarget->getAttachment());
    }

    void frameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void frameBuffer::setClearColor(color value)
    {
        _clearColor = value; 
        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    }

    void frameBuffer::setViewport(GLuint x, GLuint y, sizef size)
    {	
        _x = x;
        _y = y;
        _size = size;
    }

    void frameBuffer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void frameBuffer::enable(GLenum value)
    {
        glEnable(value);
    }

    void frameBuffer::blit(std::string renderTargetName, GLuint x, GLuint y, GLsizei width, GLsizei height, GLbitfield mask, GLenum filter)
    {
        auto renderTarget = (*_renderTargets)[renderTargetName];

        glReadBuffer(renderTarget->getAttachment());

        glBlitFramebuffer(
            0,
            0,
            (GLuint)_size.w,
            (GLuint)_size.h,
            x, 
            y, 
            x + width, 
            y + height, 
            mask, 
            filter);
    }

    void frameBuffer::blita(GLuint x, GLuint y, GLsizei width, GLsizei height, GLbitfield mask, GLenum filter)
    {
        glBlitFramebuffer(
            0,
            0,
            (GLuint)_size.w,
            (GLuint)_size.h,
            x, 
            y, 
            x + width, 
            y + height, 
            mask, 
            filter);
    }

    bool frameBuffer::addRenderTarget(renderTarget* renderTarget)
    {
        if (_renderTargets->find(renderTarget->getName()) != _renderTargets->end())
            return false;

        (*_renderTargets)[renderTarget->getName()] = renderTarget;

        if (renderTarget->getTexture()->type == GL_TEXTURE_2D)
        {
            glFramebufferTexture2D(
                renderTarget->getTarget(), 
                renderTarget->getAttachment(), 
                renderTarget->getTexTarget(),
                renderTarget->getTexture()->id,
                renderTarget->getLevel());
        }
        else  if (renderTarget->getTexture()->type == GL_TEXTURE_CUBE_MAP)
        {
            for (GLuint i = 0; i < 6; ++i)
            {
                //glFramebufferTextureARB(GL_FRAMEBUFFER, renderTarget->getAttachment(), renderTarget->getTexture()->getId(), 0);
                //glFramebufferTextureARB(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, tColorCubeMap, 0);
                
                glFramebufferTexture2D(GL_FRAMEBUFFER, renderTarget->getAttachment(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, renderTarget->getTexture()->id, renderTarget->getLevel());
            }
        }

        if (!isComplete())
            return false;

        return true;
    }

    renderTarget* frameBuffer::getRenderTarget(std::string name)
    {
        if (_renderTargets->find(name) == _renderTargets->end())
            return NULL;
        else
            return (*_renderTargets)[name];
    }

    renderTarget* frameBuffer::newRenderTarget(
        std::string name, 
        texture* texture, 
        GLenum target,
        GLenum attachment,
        GLenum texTarget,
        GLuint level)
    {
        auto r = new renderTarget(name, texture);

        r->setTarget(target);
        r->setAttachment(attachment);
        r->setTexTarget(texTarget);
        r->setLevel(level);

        return r;
    }

    GLfloat frameBuffer::getZBufferValue(vec2 mousePos)
    {
        GLfloat zBufferValue;
        glReadPixels((GLint)mousePos.x, (GLint)(_size.h - mousePos.y), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);

        return zBufferValue;
    }
}