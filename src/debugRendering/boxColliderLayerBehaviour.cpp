#include <precompiled.h>
#include "boxColliderLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include <debugRendering\boxColliderLinesRenderPass.h>
#include <debugRendering\boxColliderBoxRenderPass.h>
#include <core\mesh.h>

namespace phi
{
    boxColliderLayerBehaviour::boxColliderLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new boxColliderRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        //auto boxColliderLinesRenderPass = boxColliderLinesRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);
        auto boxColliderBoxRenderPass = boxColliderBoxRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { boxColliderBoxRenderPass };
    }

    boxColliderLayerBehaviour::~boxColliderLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void boxColliderLayerBehaviour::onNodeAdded(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _adapter->add(boxCollider);
    }

    void boxColliderLayerBehaviour::onNodeRemoved(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _adapter->remove(boxCollider);
    }

    void boxColliderLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _adapter->update(boxCollider);
    }

    void boxColliderLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }
}