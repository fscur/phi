#include <precompiled.h>
#include "framebuffer.h"

#include <core\invalidInitializationException.h>

#include "texturesManager.h"

namespace phi
{
    framebuffer* framebuffer::defaultFramebuffer = new framebuffer(true);
    framebuffer* framebuffer::_pickingFramebuffer = nullptr;
    renderTarget* framebuffer::_pickingRenderTarget = nullptr;

    framebuffer::framebuffer(bool isDefaultFramebuffer) :
        _id(0),
        _maxColorAttachments(0),
        _currentAttachment(0),
        _drawBuffers(vector<GLenum>()),
        _isDefaultFramebuffer(true)
    {
    }

    framebuffer::framebuffer() :
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
    }

    void framebuffer::attachRenderTargets()
    {
        for (auto& pair : _renderTargetsAttachments)
        {
            auto renderTarget = pair.first;
            auto attachment = pair.second;

            if (_isDefaultFramebuffer)
                throw argumentException("Trying to add renderTarget to default framebuffer !?");

            if (!(attachment == GL_DEPTH_ATTACHMENT ||
                attachment == GL_STENCIL_ATTACHMENT ||
                attachment == GL_DEPTH_STENCIL_ATTACHMENT))
                _drawBuffers.push_back(attachment);

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
        
        //TODO: arrumar isso renderTarget->texture->w!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        if (w == -1)
            w = renderTarget->texture->w;

        if (h == -1)
            h = renderTarget->texture->h;

        glBlitFramebuffer(0, 0, renderTarget->texture->w, renderTarget->texture->h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    void framebuffer::blit(framebuffer * sourceFramebuffer, renderTarget * sourceRenderTarget, framebuffer * targetFramebuffer, renderTarget * targetRenderTarget)
    {
        sourceFramebuffer->bindForReading(sourceRenderTarget);

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
        GLfloat zBufferValue;
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);


        return zBufferValue;
    }

    void framebuffer::createPickingFramebuffer(const resolution& resolution)
    {
        _pickingFramebuffer = new framebuffer();

        auto layout = textureLayout();
        layout.dataFormat = GL_RGBA;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.internalFormat = GL_RGBA8;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_NEAREST;
        layout.magFilter = GL_NEAREST;

        auto pickingTexture = new phi::texture(
            static_cast<uint>(resolution.width),
            static_cast<uint>(resolution.height),
            layout,
            nullptr,
            true,
            false);

        auto pickingTextureAddress = texturesManager::add(pickingTexture);

        /*_pickingRenderTarget = new renderTarget(
            "pickingRenderTarget",
            static_cast<GLint>(resolution.width),
            static_cast<GLint>(resolution.height),
            layout,
            pickingTextureAddress);

        _pickingFramebuffer->add(_pickingRenderTarget, GL_COLOR_ATTACHMENT0);
        */
        throw exception("fix");
    }

    framebuffer* framebuffer::getPickingFramebuffer()
    {
        if (!_pickingFramebuffer)
            throw invalidInitializationException("pickingFramebuffer was not initialized!");

        return _pickingFramebuffer;
    }

    renderTarget* framebuffer::getPickingRenderTarget()
    {
        if (!_pickingRenderTarget)
            throw invalidInitializationException("pickingFramebuffer was not initialized!");

        return _pickingRenderTarget;
    }

    void framebuffer::resize(const resolution& resolution)
    {
        //if (_isDefaultFramebuffer)
        //    return;

        //auto i = 0;

        ///*glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glDeleteFramebuffers(1, &_id);
        //glCreateFramebuffers(1, &_id);*/

        //for (auto& renderTarget : _renderTargets)
        //{
        //    auto attachment = _renderTargetsAttachments[renderTarget];

        //    auto texture = new phi::texture(
        //        static_cast<uint>(resolution.width),
        //        static_cast<uint>(resolution.height),
        //        renderTarget->layout);

        //    auto textureAddress = texturesManager::add(texture);

        //    renderTarget->w = static_cast<GLint>(resolution.width);
        //    renderTarget->h = static_cast<GLint>(resolution.height);
        //    renderTarget->textureAddress = textureAddress;

        //    glBindFramebuffer(GL_FRAMEBUFFER, _id);

        //    glNamedFramebufferTextureLayer(
        //        _id,
        //        attachment,
        //        textureAddress.containerId,
        //        0,
        //        static_cast<GLint>(textureAddress.page));

        //    auto status = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);

        //    switch (status)
        //    {
        //    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        //        phi::debug("incomplete attachment");
        //        break;
        //    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        //        phi::debug("incomplete draw buffer");
        //        break;
        //    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        //        phi::debug("incomplete layer targets");
        //        break;
        //    default:
        //        break;
        //    }
        //}
    }
}
