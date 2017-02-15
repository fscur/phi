#include <precompiled.h>

#include <core/notImplementedException.h>
#include <core/node.h>
#include <core/vertex.h>
#include <core/geometry.h>

#include <io/path.h>

#include <sceneRendering/selectionRenderPass.h>

#include "controlRenderer.h"
#include "controlRenderPass.h"

namespace phi
{
    controlRenderer::controlRenderer(vector<renderPass*>&& renderPasses) :
        _renderPasses(renderPasses)
    {
    }

    controlRenderer::~controlRenderer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void controlRenderer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    vector<renderPass*> controlRenderer::configure(
        controlRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& resourcesPath, 
        framebufferAllocator* framebufferAllocator)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto renderPass = new controlRenderPass(renderAdapter, resolution, shadersPath, framebufferAllocator);
        return { renderPass };
    }
}