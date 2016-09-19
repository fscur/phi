#include <precompiled.h>
#include "meshLayerBehaviour.h"

#include <animation/animator.h>
#include <animation/transformAnimation.h>

#include <common/mouseInteractionComponent.h>

#include <rendering/framebufferAllocator.h>

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
            auto animator = node->getComponent<phi::animator>();
            auto animation = new transformAnimation(node->getTransform());
            animator->addAnimation(animation);

            auto click = node->getComponent<mouseInteractionComponent>();
            if (click)
            {
                click->addOnMouseEnter([animation](phi::node* n)
                {
                    auto from = n->getTransform();
                    auto to = from->clone();
                    to->setLocalSize(vec3(1.15f));
                    animation->start(from, to, 0.33f);
                });

                click->addOnMouseLeave([animation](phi::node* n)
                {
                    auto from = n->getTransform();
                    auto to = from->clone();
                    to->setLocalSize(vec3(1.0f));
                    animation->start(from, to, 0.33f);
                });
            }

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
        if (mesh)
        {
            auto modelMatrix = node->getTransform()->getModelMatrix();
            _adapter->updateTransform(mesh, modelMatrix);
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