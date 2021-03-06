#include <precompiled.h>
#include "translationPlaneGridLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include "translationPlaneGridRenderPass.h"
#include <core\mesh.h>

namespace phi
{
    translationPlaneGridLayerBehaviour::translationPlaneGridLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new translationPlaneGridRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto renderPass = new translationPlaneGridRenderPass(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { renderPass };
    }

    translationPlaneGridLayerBehaviour::~translationPlaneGridLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void translationPlaneGridLayerBehaviour::onNodeAdded(node* node)
    {
        auto planeGrid = node->getComponent<phi::translationPlaneGrid>();

        if (planeGrid)
            _adapter->add(planeGrid);
    }

    void translationPlaneGridLayerBehaviour::onNodeRemoved(node* node)
    {
        auto planeGrid = node->getComponent<phi::translationPlaneGrid>();

        if (planeGrid)
            _adapter->remove(planeGrid);
    }

    void translationPlaneGridLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto planeGrid = node->getComponent<phi::translationPlaneGrid>();

        if (planeGrid)
            _adapter->update(planeGrid);
    }

    void translationPlaneGridLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        _unused(node);
        throw notImplementedException();
    }
}