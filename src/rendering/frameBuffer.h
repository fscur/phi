#pragma once
#include <precompiled.h>
#include "renderTarget.h"

namespace phi
{
    class framebuffer
    {
    private:
        GLuint _id;
        GLint _maxColorAttachments;
        GLint _currentAttachment;
        vector<GLenum> _drawBuffers;

    public:
        framebuffer(bool isDefaultFramebuffer = false) :
            _id(0),
            _currentAttachment(0)
        {
            if (!isDefaultFramebuffer)
                glCreateFramebuffers(1, &_id);

            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_maxColorAttachments);
        }

        ~framebuffer()
        {
            glDeleteFramebuffers(1, &_id);
        }

        void add(renderTarget* renderTarget)
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

            glNamedFramebufferTextureLayer(
                _id,
                att,
                renderTarget->textureAddress.containerId,
                0,
                static_cast<GLint>(renderTarget->textureAddress.page));

            auto status = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);

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

        inline void bind(GLenum target)
        {
            glBindFramebuffer(target, _id);
        }

        inline void bindForDrawing()
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
            glDrawBuffers((GLsizei)_drawBuffers.size(), &_drawBuffers[0]);
        }

        inline void bindForDrawing(GLenum* buffers, size_t buffersCount)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
            glDrawBuffers(buffersCount, buffers);
        }

        inline void bindForReading(renderTarget* sourceRenderTarget)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
            glReadBuffer(sourceRenderTarget->attachment);
        }

        inline void unbind(GLenum target)
        {
            glBindFramebuffer(target, 0);
        }

        inline void blitToDefault(renderTarget* renderTarget)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            bindForReading(renderTarget);
            glBlitFramebuffer(0, 0, renderTarget->w, renderTarget->h, 0, 0, renderTarget->w, renderTarget->h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }

        inline void blit(framebuffer* sourceFramebuffer, renderTarget* sourceRenderTarget, framebuffer* targetFramebuffer, renderTarget* targetRenderTarget)
        {
            sourceFramebuffer->bindForReading(sourceRenderTarget);
            glBlitFramebuffer(0, 0, sourceRenderTarget->w, sourceRenderTarget->h, 0, 0, targetRenderTarget->w, targetRenderTarget->h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }

        inline GLfloat getZBufferValue(ivec2 mousePos)
        {
            GLfloat zBufferValue;
            glReadPixels(mousePos.x, mousePos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zBufferValue);

            return zBufferValue;
        }
    };
}