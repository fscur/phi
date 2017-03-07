#include <precompiled.h>
#include "meshRenderer.h"

#include <io\path.h>

#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"
#include "selectionRenderPass.h"

namespace phi
{
    vector<renderPass*> meshRenderer::configure(
        const resolution& resolution,
        const string& resourcesPath, 
        meshRenderAdapter* meshAdapter,
        framebufferAllocator* framebufferAllocator)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto gBuffer = new gBufferRenderPass(resolution, meshAdapter, shadersPath, framebufferAllocator);
        auto lighting = new lightingRenderPass(resolution, shadersPath, framebufferAllocator);
        auto selection = new selectionRenderPass(resolution, shadersPath, framebufferAllocator);
        
        return{ gBuffer, lighting, selection };
        //return{ gBufferRenderPass };
    }
}