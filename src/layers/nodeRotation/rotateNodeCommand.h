#pragma once

#include <phi.h>

#include <core/node.h>
#include <core/command.h>

#include "../layersApi.h"

namespace phi
{
    class rotateNodeCommand :
        public command
    {
    private:
        node* _node;
        quat _originalOrientation;
        quat _targetOrientation;

    public:
        LAYERS_API rotateNodeCommand(node* node, quat originalOrientation, quat targetOrientation);

        LAYERS_API virtual void execute() override;
        LAYERS_API virtual void executeUndo() override;
    };
}