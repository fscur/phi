#pragma once

#include <phi.h>

#include <core/node.h>
#include <core/command.h>

#include "../layersApi.h"

namespace phi
{
    class translateNodeCommand :
        public command
    {
    private:
        node* _node;
        vec3 _originalPosition;
        vec3 _targetPosition;

    public:
        LAYERS_API translateNodeCommand(node* node, vec3 originalPosition, vec3 targetPosition);

        LAYERS_API virtual void execute() override;
        LAYERS_API virtual void executeUndo() override;
    };
}