#include <precompiled.h>
#include "animatorLayerBehaviour.h"

namespace phi
{
    animatorLayerBehaviour::animatorLayerBehaviour() :
        _animators(vector<animator*>())
    {
    }

    animatorLayerBehaviour::~animatorLayerBehaviour()
    {
    }

    void animatorLayerBehaviour::onUpdate()
    {
        for (auto& animator : _animators)
            animator->animate();
    }

    void animatorLayerBehaviour::onNodeAdded(node* node)
    {
        auto animator = node->getComponent<phi::animator>();
        if (animator)
            _animators.push_back(animator);
    }

    void animatorLayerBehaviour::onNodeRemoved(node* node)
    {
        auto animator = node->getComponent<phi::animator>();
        if (animator)
            phi::removeIfContains(_animators, animator);
    }

    void animatorLayerBehaviour::onNodeComponentAdded(node* node, component* component)
    {
        _unused(node);

        if (component->getType() == component::componentType::ANIMATOR)
        {
            auto animator = static_cast<phi::animator*>(component);

            if (!contains(_animators, animator))
                _animators.push_back(animator);
        }
    }

    void animatorLayerBehaviour::onNodeComponentRemoved(node* node, component* component)
    {
        _unused(node);

        if (component->getType() == component::componentType::ANIMATOR)
        {
            auto animator = static_cast<phi::animator*>(component);
            removeIfContains(_animators, animator);
        }
    }
}