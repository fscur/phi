#pragma once
#include <phi.h>

#include <core/resolution.h>

#include <rendering/renderPass.h>
#include <rendering/framebufferAllocator.h>

#include "meshRenderAdapter.h"

namespace phi
{
    class gBufferRenderPass :
        public renderPass
    {
    public:
        /*gBufferRenderPass() = delete;
        static renderPass* configure(
            meshRenderAdapter* meshAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);*/

        gBufferRenderPass(
            const resolution& resolution,
            meshRenderAdapter* meshAdapter,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~gBufferRenderPass();

        void onInitialize() override;
        void onBeginRender() override;
        void onEndRender() override;

    private:
        meshRenderAdapter* _meshAdapter;
        framebufferAllocator* _framebufferAllocator;
        const string _shadersPath;
        vector<vertexArrayObject*> _vaosToRemove;
    };
}