#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\resolution.h>

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
        unordered_map<const renderTarget*, GLenum> _renderTargetsAttachments;
        vector<renderTarget*> _renderTargets;

        bool _isDefaultFramebuffer;

        static framebuffer* _pickingFramebuffer;
        static renderTarget* _pickingRenderTarget;
    public:
        RENDERING_API static framebuffer* defaultFramebuffer;

    private:
        RENDERING_API framebuffer(bool isDefaultFramebuffer);

    public:
        RENDERING_API framebuffer();
        RENDERING_API ~framebuffer();

        RENDERING_API void add(GLenum attachment, renderTarget* renderTarget);
        RENDERING_API vec4 readPixels(const renderTarget* const renderTarget, GLint x, GLint y, GLsizei w, GLsizei h);
        RENDERING_API void bind(GLenum target);
        RENDERING_API void bindForDrawing();
        RENDERING_API void bindForDrawing(GLenum* buffers, GLsizei buffersCount);
        RENDERING_API void bindForReading();
        RENDERING_API void bindForReading(const renderTarget* const sourceRenderTarget);
        RENDERING_API void unbind(GLenum target);
        RENDERING_API void blitToDefault(renderTarget* renderTarget, int x = 0, int y = 0, int w = -1, int h = -1);
        RENDERING_API void blit(
            framebuffer* sourceFramebuffer,
            renderTarget* sourceRenderTarget,
            framebuffer* targetFramebuffer,
            renderTarget* targetRenderTarget);

        RENDERING_API renderTarget* getRenderTarget(string name);
        RENDERING_API GLfloat getZBufferValue(int x, int y);
        vector<renderTarget*> getRenderTargets() { return _renderTargets; }

        RENDERING_API static void createPickingFramebuffer(const resolution& resolution);
        RENDERING_API static framebuffer* getPickingFramebuffer();
        RENDERING_API static renderTarget* getPickingRenderTarget();
        RENDERING_API void resize(const resolution& resolution);
    };
}