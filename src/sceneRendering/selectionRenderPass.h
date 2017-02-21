#pragma once
#include <phi.h>

#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

namespace phi
{
    class selectionRenderPass :
        public renderPass
    {
    public:
        selectionRenderPass(
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~selectionRenderPass();

        void onBeginRender() override;
        void onEndRender() override;

    private:
        renderTarget* _pickingRenderTarget;
        renderTarget* _defaultRenderTarget;
        vertexArrayObject* _quadVao;
    };
}
