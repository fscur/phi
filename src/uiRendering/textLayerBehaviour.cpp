#include <precompiled.h>
#include "textLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    textLayerBehaviour::textLayerBehaviour(
        const resolution & resolution, 
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator,
        textRenderAdapter* adapter) :
        _adapter(adapter),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = textRenderer::configure(_adapter, resolution, resourcesPath, framebufferAllocator);
    }

    textLayerBehaviour::~textLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void textLayerBehaviour::onNodeAdded(node* node)
    {
        auto text = node->getComponent<phi::text>();

        if (text)
            _adapter->add(text);
    }

    void textLayerBehaviour::onNodeRemoved(node* node)
    {
        auto text = node->getComponent<phi::text>();

        if (text)
            _adapter->remove(text);
    }

    void textLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto text = node->getComponent<phi::text>();

        if (text)
            _adapter->update(text);
    }

    void textLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        auto text = node->getComponent<phi::text>();

        if(text)
            throw notImplementedException();
    }
}