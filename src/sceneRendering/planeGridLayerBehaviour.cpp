#include <precompiled.h>
#include "planeGridLayerBehaviour.h"

#include <core\notImplementedException.h>
#include <io\path.h>
#include <rendering\framebufferAllocator.h>
#include "planeGridRenderPass.h"
#include <core\mesh.h>

namespace phi
{
    planeGridLayerBehaviour::planeGridLayerBehaviour(
        const resolution & resolution,
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator) :
        _adapter(new planeGridRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto boxRenderPass = planeGridRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { boxRenderPass };
    }

    planeGridLayerBehaviour::~planeGridLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void planeGridLayerBehaviour::onNodeAdded(node* node)
    {
        auto planeGrid = node->getComponent<phi::planeGrid>();

        if (planeGrid)
            _adapter->add(planeGrid);
    }

    void planeGridLayerBehaviour::onNodeRemoved(node* node)
    {
        auto planeGrid = node->getComponent<phi::planeGrid>();

        if (planeGrid)
            _adapter->remove(planeGrid);
    }

    void planeGridLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto planeGrid = node->getComponent<phi::planeGrid>();

        if (planeGrid)
            _adapter->update(planeGrid);
    }

    void planeGridLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        _unused(node);

        throw notImplementedException();
    }
}