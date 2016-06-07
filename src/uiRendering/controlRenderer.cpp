#include <precompiled.h>
#include "controlRenderer.h"

#include <core\notImplementedException.h>
#include <core\node.h>
#include <core\vertex.h>
#include <core\geometry.h>

#include <io\path.h>

#include "controlRenderPassConfigurator.h"

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

    vector<renderPass*> controlRenderer::configure(gl* gl, float width, float height, const string& resourcesPath, controlRendererDescriptor* renderDescriptor)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto controlRenderPass = controlRenderPassConfigurator::configureNewControlRenderPass(renderDescriptor, gl, width, height, shadersPath);
        return {controlRenderPass};
    }
}