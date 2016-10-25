#include <precompiled.h>
#include "framebuffer.h"

#include <core\invalidInitializationException.h>

#include "texturesManager.h"
#include "framebufferLayoutBuilder.h"

namespace phi
{
    framebuffer* framebuffer::defaultFramebuffer = new framebuffer(true);

    framebuffer::framebuffer(bool isDefaultFramebuffer) :
        _name("defaultFramebuffer"),
        _id(0),
        _maxColorAttachments(0),
        _currentAttachment(0),
        _drawBuffers(vector<GLenum>()),
        _isDefaultFramebuffer(isDefaultFramebuffer)
    {
    }

    framebuffer::framebuffer(const string& name) :
        _name(name),
        _id(0),
        _maxColorAttachments(0),
        _currentAttachment(0),
        _drawBuffers(vector<GLenum>()),
        _isDefaultFramebuffer(false)
    {
        glCreateFramebuffers(1, &_id);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_maxColorAttachments);
    }

    framebuffer::~framebuffer()
    {
        if (!_isDefaultFramebuffer)
            glDeleteFramebuffers(1, &_id);
    }

    void framebuffer::add(renderTarget* renderTarget, GLenum attachment)
    {
        _renderTargets.push_back(renderTarget);
        _renderTargetsAttachments[renderTarget] = attachment;

        if (!(attachment == GL_DEPTH_ATTACHMENT ||
            attachment == GL_STENCIL_ATTACHMENT ||
            attachment == GL_DEPTH_STENCIL_ATTACHMENT))
            _drawBuffers.push_back(attachment);
    }

    void framebuffer::attachRenderTargets()
    {
        for (auto& pair : _renderTargetsAttachments)
        {
            auto renderTarget = pair.first;
            auto attachment = pair.second;

            if (_isDefaultFramebuffer)
                throw argumentException("Trying to add renderTarget to default framebuffer !?");

            glBindFramebuffer(GL_FRAMEBUFFER, _id);

            auto address = texturesManager::getTextureAddress(renderTarget->texture);

            glNamedFramebufferTextureLayer(
                _id,
                attachment,
                address.containerId,
                0,
                static_cast<GLint>(address.page));
        }
    }

    vec4 framebuffer::readPixels(const renderTarget* const renderTarget, GLint x, GLint y, GLsizei w, GLsizei h)
    {
        unsigned char pixels[4];

        bindForReading(renderTarget);
        glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        return vec4(pixels[0], pixels[1], pixels[2], pixels[3]);
    }

    void framebuffer::bind(GLenum target)
    {
        glBindFramebuffer(target, _id);

    }

    void framebuffer::bindForDrawing()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);

        if (!_isDefaultFramebuffer)
            glDrawBuffers((GLsizei)_drawBuffers.size(), &_drawBuffers[0]);
    }

    void framebuffer::bindForDrawing(GLenum* buffers, GLsizei buffersCount)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
        glDrawBuffers(buffersCount, buffers);

    }

    void framebuffer::bindForReading()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
    }

    void framebuffer::bindForReading(const renderTarget* const sourceRenderTarget)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
        glReadBuffer(_renderTargetsAttachments[sourceRenderTarget]);
    }

    void framebuffer::unbind(GLenum target)
    {
        glBindFramebuffer(target, 0);
    }

    void framebuffer::blitToDefault(renderTarget * renderTarget, int x, int y, int w, int h)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        bindForReading(renderTarget);

        glBlitFramebuffer(0, 0, renderTarget->texture->w, renderTarget->texture->h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    void framebuffer::blit(renderTarget * sourceRenderTarget, renderTarget * targetRenderTarget)
    {
        bindForReading(sourceRenderTarget);

        glBlitFramebuffer(
            0,
            0,
            sourceRenderTarget->texture->w,
            sourceRenderTarget->texture->h,
            0,
            0,
            targetRenderTarget->texture->w,
            targetRenderTarget->texture->h,
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR);
    }

    renderTarget* framebuffer::getRenderTarget(string name)
    {
        for (auto& renderTarget : _renderTargets)
        {
            if (renderTarget->name == name)
                return renderTarget;
        }

        throw exception("renderTarget " + name + " not found.");
    }

    GLfloat framebuffer::getZBufferValue(int x, int y)
    {
        bindForReading();
        GLfloat zBufferValue = -1.0f;
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);

        
        //debug(std::to_string(zBufferValue));

        return zBufferValue;
    }

    void framebuffer::release()
    {
        safeDelete(defaultFramebuffer);
    }
}
