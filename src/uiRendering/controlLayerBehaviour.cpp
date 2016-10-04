#include <precompiled.h>
#include "controlLayerBehaviour.h"

#include <core\notImplementedException.h>

namespace phi
{
    controlLayerBehaviour::controlLayerBehaviour(
        const resolution & resolution, 
        const string & resourcesPath,
        framebufferAllocator* framebufferAllocator,
        controlRenderAdapter* adapter,
        vector<renderPass*> renderPasses) :
        _adapter(adapter),
        _resolution(resolution),
        _resourcesPath(resourcesPath),
        _renderPasses(renderPasses)
    {
    }

    controlLayerBehaviour::~controlLayerBehaviour()
    {
        safeDelete(_adapter);

        for (auto& renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void controlLayerBehaviour::onNodeAdded(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
        {
            control->colorChanged.assign(std::bind(&controlLayerBehaviour::onControlColorChanged, this, std::placeholders::_1));
            _adapter->add(control);
        }
    }

    void controlLayerBehaviour::onNodeRemoved(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->remove(control);
    }

    void controlLayerBehaviour::onNodeTransformChanged(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            _adapter->updateModelMatrix(control);
    }

    void controlLayerBehaviour::onControlColorChanged(control* control)
    {
        _adapter->update(control);
    }
}