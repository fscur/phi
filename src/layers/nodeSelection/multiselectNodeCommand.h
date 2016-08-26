#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class multiselectNodeCommand :
        public command
    {
    private:
        node* _nodeToSelect;
    
    private:
        node * getSonOfRoot(node* node);
    public:
        LAYERS_API multiselectNodeCommand(node* nodeToSelect);
        LAYERS_API ~multiselectNodeCommand();

        LAYERS_API virtual void execute();
        LAYERS_API virtual void executeUndo();
    };
}