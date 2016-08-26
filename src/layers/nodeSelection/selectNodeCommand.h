#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\command.h>
#include <core\node.h>

#include "unselectNodeCommand.h"

namespace phi
{
    class selectNodeCommand :
        public command
    {
    private:
        node* _nodeToSelect;

    public:
        LAYERS_API selectNodeCommand(node* nodeToSelect);
        LAYERS_API ~selectNodeCommand();
        
        LAYERS_API virtual void execute();
        LAYERS_API virtual void executeUndo();
    };
}