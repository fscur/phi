#include <precompiled.h>
#include "glassyControlRenderer.h"

#include <core\notImplementedException.h>
#include <core\node.h>
#include <core\vertex.h>
#include <core\geometry.h>

#include <io\path.h>

#include "glassyControlRenderPass.h"

namespace phi
{
    glassyControlRenderer::glassyControlRenderer(vector<renderPass*>&& renderPasses) :
        _renderPasses(renderPasses)
    {
    }

    glassyControlRenderer::~glassyControlRenderer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void glassyControlRenderer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    vector<renderPass*> glassyControlRenderer::configure(
        const string& resourcesPath, 
        glassyControlRendererDescriptor* renderDescriptor)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto glassyControlRenderPass = glassyControlRenderPass::configure(renderDescriptor, shadersPath);

        return { glassyControlRenderPass };
    }
}