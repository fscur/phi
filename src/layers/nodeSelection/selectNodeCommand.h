#pragma once
#include <phi.h>

#include <core/command.h>
#include <core/node.h>

#include "../layersApi.h"
#include "unselectNodeCommand.h"

namespace phi
{
    class selectNodeCommand :
        public command
    {
    public:
        LAYERS_API selectNodeCommand(node* nodeToSelect);
        LAYERS_API ~selectNodeCommand();
        
        LAYERS_API virtual void execute();
        LAYERS_API virtual void executeUndo();

    private:
        node* _nodeToSelect;
    };
}