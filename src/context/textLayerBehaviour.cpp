#include <precompiled.h>
#include "textLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    textLayerBehaviour::textLayerBehaviour(const resolution & resolution, const string & resourcesPath) :
        _descriptor(new textRendererDescriptor()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = textRenderer::configure(resourcesPath, _descriptor);
    }

    textLayerBehaviour::~textLayerBehaviour()
    {
        safeDelete(_descriptor);
    }

    void textLayerBehaviour::onNodeAdded(node* node)
    {
        auto text = node->getComponent<phi::text>();
        if (text)
        {
            _descriptor->add(text);
        }
    }

    void textLayerBehaviour::onNodeRemoved(node* node)
    {
        auto text = node->getComponent<phi::text>();
        if (text)
        {
            _descriptor->remove(text);
        }
    }

    void textLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto text = node->getComponent<phi::text>();
        if (text)
            _descriptor->update(text);
    }

    void textLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }
}