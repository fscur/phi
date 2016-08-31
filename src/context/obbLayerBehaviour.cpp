#include <precompiled.h>
#include "obbLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include <debugRendering\obbLinesRenderPass.h>
#include <debugRendering\obbBoxRenderPass.h>
#include <core\mesh.h>

namespace phi
{
    obbLayerBehaviour::obbLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new obbRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        //auto linesRenderPass = obbLinesRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);
        auto boxRenderPass = obbBoxRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { boxRenderPass };
    }

    obbLayerBehaviour::~obbLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    bool obbLayerBehaviour::shouldShowObbOf(node* node)
    {
        auto hasObb = node->getObb() != nullptr;
        if (!hasObb)
            return false;

        auto hasParent = node->getParent() != nullptr;
        return hasParent;
    }

    void obbLayerBehaviour::onNodeAdded(node* node)
    {
        if (shouldShowObbOf(node))
            _adapter->add(node);
    }

    void obbLayerBehaviour::onNodeRemoved(node* node)
    {
        if (shouldShowObbOf(node))
            _adapter->remove(node);
    }

    void obbLayerBehaviour::onNodeTransformChanged(node* node)
    {
        if (shouldShowObbOf(node))
            _adapter->update(node);
    }

    void obbLayerBehaviour::onNodeObbChanged(node* node)
    {
        if (shouldShowObbOf(node))
            _adapter->update(node);
    }
}