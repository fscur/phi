#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class deselectNodeCommand :
        public command
    {
    private:
        vector<node*> _nodesToDeselect;
        vector<node*> _deselectedNodes;
    public:
        LAYERS_API deselectNodeCommand(const vector<node*>& nodesToDeselect);
        LAYERS_API ~deselectNodeCommand();

        LAYERS_API virtual void execute();
        LAYERS_API virtual void executeUndo();
    };
}