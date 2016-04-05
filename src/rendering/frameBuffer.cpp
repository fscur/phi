#include "precompiled.h"
#include "frameBuffer.h"
#include "glError.h"

namespace phi
{
    framebuffer::framebuffer(bool isDefaultFramebuffer) :
        _id(0),
        _maxColorAttachments(0),
        _currentAttachment(0),
        _drawBuffers(vector<GLenum>())
    {
        if (!isDefaultFramebuffer)
        {
            glCreateFramebuffers(1, &_id);
            glError::check();
        }

        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_maxColorAttachments);
        glError::check();
    }

    framebuffer::~framebuffer()
    {
        glDeleteFramebuffers(1, &_id);
        glError::check();
    }

    void framebuffer::add(renderTarget* renderTarget)
    {
        if (_id == 0)
        {
            phi::debug("trying to add render target to default framebuffer!?");
            return;
        }

        auto att = renderTarget->attachment;

        if (!(att == GL_DEPTH_ATTACHMENT ||
            att == GL_STENCIL_ATTACHMENT ||
            att == GL_DEPTH_STENCIL_ATTACHMENT))
            _drawBuffers.push_back(att);

        glBindFramebuffer(GL_FRAMEBUFFER, _id);
        glError::check();

        glNamedFramebufferTextureLayer(
            _id,
            att,
            renderTarget->textureAddress.containerId,
            0,
            static_cast<GLint>(renderTarget->textureAddress.page));
        glError::check();

        auto status = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);
        glError::check();

        switch (status)
        {
            case GL_FRAMEBUFFER_COMPLETE:
                phi::debug("complete");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                phi::debug("incomplete attachment");;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                phi::debug("incomplete draw buffer");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                phi::debug("incomplete layer targets");
                break;
            default:
                break;
        }
    }

    void framebuffer::bind(GLenum target)
    {
        glBindFramebuffer(target, _id);
        glError::check();
    }

    void framebuffer::bindForDrawing()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
        glError::check();

        glDrawBuffers((GLsizei)_drawBuffers.size(), &_drawBuffers[0]);
        glError::check();
    }

    void framebuffer::bindForDrawing(GLenum * buffers, GLsizei buffersCount)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
        glError::check();

        glDrawBuffers(buffersCount, buffers);
        glError::check();
    }

    void framebuffer::bindForReading()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
        glError::check();
    }

    void framebuffer::bindForReading(renderTarget * sourceRenderTarget)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
        glError::check();

        glReadBuffer(sourceRenderTarget->attachment);
        glError::check();
    }

    void framebuffer::unbind(GLenum target)
    {
        glBindFramebuffer(target, 0);
        glError::check();
    }

    void framebuffer::blitToDefault(renderTarget * renderTarget)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glError::check();

        bindForReading(renderTarget);
        glError::check();

        glBlitFramebuffer(0, 0, renderTarget->w, renderTarget->h, 0, 0, renderTarget->w, renderTarget->h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glError::check();
    }

    void framebuffer::blit(framebuffer * sourceFramebuffer, renderTarget * sourceRenderTarget, framebuffer * targetFramebuffer, renderTarget * targetRenderTarget)
    {
        sourceFramebuffer->bindForReading(sourceRenderTarget);

        glBlitFramebuffer(
            0,
            0,
            sourceRenderTarget->w,
            sourceRenderTarget->h,
            0,
            0,
            targetRenderTarget->w,
            targetRenderTarget->h,
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR);

        glError::check();
    }

    GLfloat framebuffer::getZBufferValue(int x, int y)
    {
        GLfloat zBufferValue;

        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);
        glError::check();

        return zBufferValue;
    }
}
