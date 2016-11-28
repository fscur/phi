#include <precompiled.h>
#include "boxColliderLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include <debugRendering\boxColliderRenderPass.h>
#include <core\boxCollider.h>

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
        //auto linesRenderPass = obbLinesRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);
        auto boxRenderPass = boxColliderRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { boxRenderPass };
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

    void boxColliderLayerBehaviour::onNodeComponentAdded(node* node, component* component)
    {
        _unused(node);

        if (component->getType() == component::componentType::BOX_COLLIDER)
        {
            auto collider = static_cast<phi::boxCollider*>(component);

            if (!_adapter->contains(collider))
                _adapter->add(collider);
        }
    }

    void boxColliderLayerBehaviour::onNodeComponentRemoved(node* node, component* component)
    {
        _unused(node);

        if (component->getType() == component::componentType::BOX_COLLIDER)
        {
            auto animator = static_cast<phi::boxCollider*>(component);
            _adapter->remove(animator);
        }
    }
}