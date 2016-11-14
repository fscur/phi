#include <precompiled.h>
#include "rotationPlaneGridLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include "rotationPlaneGridRenderPass.h"
#include <core\mesh.h>

namespace phi
{
    rotationPlaneGridLayerBehaviour::rotationPlaneGridLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new rotationPlaneGridRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto boxRenderPass = rotationPlaneGridRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { boxRenderPass };
    }

    rotationPlaneGridLayerBehaviour::~rotationPlaneGridLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void rotationPlaneGridLayerBehaviour::onNodeAdded(node* node)
    {
        auto planeGrid = node->getComponent<phi::rotationPlaneGrid>();

        if (planeGrid)
            _adapter->add(planeGrid);
    }

    void rotationPlaneGridLayerBehaviour::onNodeRemoved(node* node)
    {
        auto planeGrid = node->getComponent<phi::rotationPlaneGrid>();

        if (planeGrid)
            _adapter->remove(planeGrid);
    }

    void rotationPlaneGridLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto planeGrid = node->getComponent<phi::rotationPlaneGrid>();

        if (planeGrid)
            _adapter->update(planeGrid);
    }

    void rotationPlaneGridLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        _unused(node);
        throw notImplementedException();
    }
}