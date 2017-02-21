#pragma once
#include <phi.h>
#include "animationApi.h"

#include <core\eventArgs.h>
#include <core\node.h>

#include <animation\animator.h>

namespace phi
{
    class animatorLayerBehaviour
    {
    private:
        vector<animator*> _animators;

    public:
        ANIMATION_API animatorLayerBehaviour();
        ANIMATION_API ~animatorLayerBehaviour();
        
        ANIMATION_API void onUpdate();
        ANIMATION_API void onNodeAdded(node* node);
        ANIMATION_API void onNodeRemoved(node* node);
        ANIMATION_API void onNodeComponentAdded(node* node,  component* component);
        ANIMATION_API void onNodeComponentRemoved(node* node, component* component);
    };
}