#include <precompiled.h>
#include "textRenderer.h"

#include <core\resolution.h>
#include <core\node.h>
#include <core\vertex.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>

#include <io\path.h>

#include "textRenderPass.h"

namespace phi
{
    textRenderer::textRenderer(vector<renderPass*>&& renderPasses) :
        _renderPasses(renderPasses)
    {
    }

    textRenderer::~textRenderer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void textRenderer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    vector<renderPass*> textRenderer::configure(const string& resourcesPath, textRendererDescriptor* renderDescriptor)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto textRenderPass = textRenderPass::configure(renderDescriptor, shadersPath);
        return { textRenderPass };
    }
}