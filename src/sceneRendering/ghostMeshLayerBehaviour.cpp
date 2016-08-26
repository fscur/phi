#include <precompiled.h>
#include "ghostMeshLayerBehaviour.h"

#include <sceneRendering\ghostMeshRenderPass.h>

#include <core\clickComponent.h>

#include <rendering\framebufferAllocator.h>
#include <io\path.h>

namespace phi
{
    ghostMeshLayerBehaviour::ghostMeshLayerBehaviour(
        const resolution & resolution, 
        const string & resourcesPath, 
        framebufferAllocator* framebufferAllocator) :
        _adapter(new ghostMeshRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto ghostMeshRenderPass = ghostMeshRenderPass::configure(_adapter, resolution, shadersPath, framebufferAllocator);

        _renderPasses = { ghostMeshRenderPass };
    }

    ghostMeshLayerBehaviour::~ghostMeshLayerBehaviour()
    {
        safeDelete(_adapter);
        
        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void ghostMeshLayerBehaviour::onNodeAdded(node* node)
    {
        auto ghostMesh = node->getComponent<phi::ghostMesh>();
        if (ghostMesh)
        {
            _adapter->add(ghostMesh);
        }
    }

    void ghostMeshLayerBehaviour::onNodeRemoved(node* node)
    {
        auto mesh = node->getComponent<phi::ghostMesh>();
        if (mesh)
        {
            _adapter->remove(mesh);
        }
    }

    void ghostMeshLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto ghostMesh = node->getComponent<phi::ghostMesh>();
        if (ghostMesh)
        {
            auto modelMatrix = node->getTransform()->getModelMatrix();
            _adapter->update(ghostMesh);
        }
    }
}