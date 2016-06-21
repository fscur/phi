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
        _descriptor(new meshRendererDescriptor()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = meshRenderer::configure(resolution, resourcesPath, _descriptor, framebufferAllocator);
    }

    meshLayerBehaviour::~meshLayerBehaviour()
    {
        safeDelete(_descriptor);
    }

    void meshLayerBehaviour::onNodeAdded(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            _descriptor->add(mesh);
        }

        auto click = node->getComponent<phi::clickComponent>();
        if (click)
        {
            click->addOnClick([](phi::node* lamdaNode)
            {
                lamdaNode->setIsSelected(true);
            });
        }
    }

    void meshLayerBehaviour::onNodeRemoved(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            _descriptor->remove(mesh);
        }
    }

    void meshLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            auto modelMatrix = node->getTransform()->getModelMatrix();
            _descriptor->updateTransform(mesh, modelMatrix);
        }
    }

    void meshLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            _descriptor->updateSelection(mesh, node->getIsSelected());
        }
    }
}