#include <precompiled.h>
#include "controlLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    controlLayerBehaviour::controlLayerBehaviour(const resolution & resolution, const string & resourcesPath) :
        _descriptor(new controlRendererDescriptor()),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
        _renderPasses = controlRenderer::configure(resourcesPath, _descriptor);
    }

    controlLayerBehaviour::~controlLayerBehaviour()
    {
        safeDelete(_descriptor);
    }

    void controlLayerBehaviour::onNodeAdded(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _descriptor->add(control);
    }

    void controlLayerBehaviour::onNodeRemoved(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _descriptor->remove(control);
    }

    void controlLayerBehaviour::onNodeTransformChanged(node* node)
    {
        throw notImplementedException();
    }

    void controlLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        throw notImplementedException();
    }
}