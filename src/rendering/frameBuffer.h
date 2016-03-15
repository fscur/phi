#pragma once
#include <phi.h>
#include "rendering.h"
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
        RENDERING_API framebuffer(bool isDefaultFramebuffer = false);
        RENDERING_API ~framebuffer();

        RENDERING_API void add(renderTarget* renderTarget);
        RENDERING_API void bind(GLenum target);
        RENDERING_API void bindForDrawing();
        RENDERING_API void bindForDrawing(GLenum* buffers, GLsizei buffersCount);
        RENDERING_API void bindForReading(renderTarget* sourceRenderTarget);
        RENDERING_API void unbind(GLenum target);
        RENDERING_API void blitToDefault(renderTarget* renderTarget);
        RENDERING_API void blit(
            framebuffer* sourceFramebuffer,
            renderTarget* sourceRenderTarget,
            framebuffer* targetFramebuffer,
            renderTarget* targetRenderTarget);

        RENDERING_API GLfloat getZBufferValue(int x, int y);
    };
}