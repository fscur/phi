#include <precompiled.h>
#include "meshRenderer.h"

#include <core\notImplementedException.h>
#include <core\node.h>

#include <io\path.h>

#include <rendering\programBuilder.h>

#include "gBufferRenderPassConfigurator.h"
#include "lightingRenderPassConfigurator.h"

namespace phi
{
    meshRenderer::meshRenderer(vector<renderPass*>&& renderPasses) :
        _renderPasses(renderPasses)
    {
    }

    meshRenderer::~meshRenderer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void meshRenderer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    vector<renderPass*> meshRenderer::configure(
        gl* gl, 
        float width, 
        float height, 
        const string& resourcesPath, 
        meshRendererDescriptor* rendererDescriptor)
    {
        auto shadersPath = path::combine(resourcesPath, "/shaders");
        auto gBufferRenderPass = gBufferRenderPassConfigurator::configureNewGBuffer(rendererDescriptor, gl, width, height, shadersPath);
        auto lightingRenderPass = lightingRenderPassConfigurator::configureNewLighting(gBufferRenderPass, gl, width, height, shadersPath);

        return { gBufferRenderPass, lightingRenderPass };
    }
}