#pragma once
#include <phi.h>
#include "contextApi.h"

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
        CONTEXT_API animatorLayerBehaviour();
        CONTEXT_API ~animatorLayerBehaviour();

        CONTEXT_API void onUpdate();
        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
    };
}