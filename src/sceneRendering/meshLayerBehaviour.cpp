#include <precompiled.h>
#include "meshLayerBehaviour.h"

#include <core\clickComponent.h>

#include <rendering\framebufferAllocator.h>

namespace phi
{
    meshLayerBehaviour::meshLayerBehaviour(
        const resolution & resolution, 
        const string & resourcesPath, 
        framebufferAllocator* framebufferAllocator) :
        _adapter(new meshRenderAdapter()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = meshRenderer::configure(resolution, resourcesPath, _adapter, framebufferAllocator);
    }

    meshLayerBehaviour::~meshLayerBehaviour()
    {
        safeDelete(_adapter);
        
        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void meshLayerBehaviour::onNodeAdded(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            _adapter->add(mesh);
        }
    }

    void meshLayerBehaviour::onNodeRemoved(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            _adapter->remove(mesh);
        }
    }

    void meshLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        auto isSelected = false;
        auto currentNode = node;
        while (currentNode != nullptr && !isSelected)
        {
            if (currentNode->isSelected())
                isSelected = true;

            currentNode = currentNode->getParent();
        }

        if (mesh)
        {
            auto modelMatrix = node->getTransform()->getModelMatrix();
            _adapter->updateTransform(mesh, modelMatrix);
            _adapter->updateSelection(mesh, isSelected);
        }
    }

    void meshLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        auto isSelected = node->isSelected();
        node->traverse<mesh>([&] (mesh* mesh)
        {
            _adapter->updateSelection(mesh, isSelected);
        });
    }
}