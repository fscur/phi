#include <precompiled.h>
#include "textLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    textLayerBehaviour::textLayerBehaviour(
        textRenderAdapter* adapter,
        const resolution & resolution, 
        const string & resourcesPath):
        _adapter(adapter),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = textRenderer::configure(_adapter, resolution, resourcesPath);
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
            _adapter->updateTransform(text);
    }

    void textLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        auto text = node->getComponent<phi::text>();

        if(text)
            throw notImplementedException();
    }
}