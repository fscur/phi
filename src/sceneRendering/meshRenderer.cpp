#include <precompiled.h>
#include "meshRenderer.h"

#include <core\notImplementedException.h>
#include <core\node.h>

#include <io\path.h>

#include <rendering\texturesManager.h>
#include <rendering\programBuilder.h>

#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"
#include "selectionRenderPass.h"

namespace phi
{
    vector<renderPass*> meshRenderer::configure(
        const resolution& resolution,
        const string& resourcesPath, 
        meshRendererDescriptor* rendererDescriptor,
        framebufferAllocator* framebufferAllocator)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto gBufferRenderPass = gBufferRenderPass::configure(rendererDescriptor, resolution, shadersPath, framebufferAllocator);
        auto lightingRenderPass = lightingRenderPass::configure(gBufferRenderPass, resolution, shadersPath, framebufferAllocator);
        //auto selectionRenderPass = selectionRenderPass::configure(lightingRenderPass, resolution, shadersPath);
        //return{ gBufferRenderPass};
        return{ gBufferRenderPass, lightingRenderPass }; // , lightingRenderPass, selectionRenderPass
    }
}