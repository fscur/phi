#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class unselectNodeCommand :
        public command
    {
    private:
        vector<node*> _nodesToUnselect;
        vector<node*> _unselectedNodes;
    public:
        LAYERS_API unselectNodeCommand(const vector<node*>& nodesToUnselect);
        LAYERS_API ~unselectNodeCommand();

        LAYERS_API virtual void execute();
        LAYERS_API virtual void executeUndo();
    };
}