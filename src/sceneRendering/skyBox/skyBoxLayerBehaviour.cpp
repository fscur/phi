#include <precompiled.h>

#include <core\notImplementedException.h>
#include <core\mesh.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>

#include "skyBoxRenderPass.h"
#include "skyBoxLayerBehaviour.h"

namespace phi
{
    skyBoxLayerBehaviour::skyBoxLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new skyBoxRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto skyBox = new skyBoxRenderPass(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { skyBox };
    }

    skyBoxLayerBehaviour::~skyBoxLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void skyBoxLayerBehaviour::onNodeAdded(node* node)
    {
        auto skyBox = node->getComponent<phi::skyBox>();

        if (skyBox)
            _adapter->add(skyBox);
    }

    void skyBoxLayerBehaviour::onNodeRemoved(node* node)
    {
        auto skyBox = node->getComponent<phi::skyBox>();

        if (skyBox)
            _adapter->remove(skyBox);
    }

    void skyBoxLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto skyBox = node->getComponent<phi::skyBox>();

        if (skyBox)
            _adapter->update(skyBox);
    }
}