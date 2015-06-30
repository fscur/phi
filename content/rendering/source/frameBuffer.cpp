#include "frameBuffer.h"

namespace phi
{
    frameBuffer::frameBuffer(std::string name, size<GLuint> size, color clearColor)
    {
        _name = name;
        _size = size;
        _clearColor = clearColor;
        _x = 0;
        _y = 0;
        _isBound = false;
        _isInitialized = false;
        _id = 0;
        _renderTargets = new std::map<std::string, renderTarget*>();
    }

    frameBuffer::~frameBuffer()
    {
        if (_renderTargets)
        {
            for(std::map<std::string, renderTarget*>::iterator i = _renderTargets->begin(); i != _renderTargets->end(); i++) 
            {
                DELETE(i->second);
            }

            DELETE(_renderTargets);
        }

        glDeleteFramebuffers(1, &_id);
    }

    void frameBuffer::init()
    {
        glGenFramebuffers(1, &_id);
        glBindFramebuffer(GL_FRAMEBUFFER, _id);

        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
        glViewport(_x, _y, _size.width, _size.height);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool frameBuffer::isComplete(GLenum target)
    {
        GLenum status = glCheckFramebufferStatus(target);

        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string msg = &"FB error, status: 0x%x\n" [status];
            LOG(msg);
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
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, cubeMapRenderTarget->getAttachment(), cubeMapFace, cubeMapRenderTarget->getTexture()->getId(), 0);
        glDrawBuffer(cubeMapRenderTarget->getAttachment());
    }

    void frameBuffer::bindForReading()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
    }

    void frameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void frameBuffer::setSize(size<GLuint> value)
    {
        _size = value;

        glViewport(0,0, _size.width, _size.height);
    }

    void frameBuffer::setClearColor(color value)
    {
        _clearColor = value; 
        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    }

    void frameBuffer::setViewport(GLuint x, GLuint y, size<GLuint> size)
    {	
        _x = x;
        _y = y;
        _size = size;

        glViewport(_x, _y, _size.width, _size.height);
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
        renderTarget* renderTarget = (*_renderTargets)[renderTargetName];

        glReadBuffer(renderTarget->getAttachment());

        glBlitFramebuffer(
            0,
            0,
            _size.width,
            _size.height,
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

        if (renderTarget->getTexture()->getTextureType() == GL_TEXTURE_2D)
        {
            glFramebufferTexture2D(
                renderTarget->getTarget(), 
                renderTarget->getAttachment(), 
                renderTarget->getTexTarget(),
                renderTarget->getTexture()->getId(),
                renderTarget->getLevel());
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
        renderTarget* r = new renderTarget(name, texture);

        r->setTarget(target);
        r->setAttachment(attachment);
        r->setTexTarget(texTarget);
        r->setLevel(level);

        return r;
    }

    GLfloat frameBuffer::getZBufferValue(glm::vec2 mousePos)
    {
        GLfloat zBufferValue;
        glReadPixels((GLint)mousePos.x, (GLint)(_size.height - mousePos.y), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);

        return zBufferValue;
    }
}