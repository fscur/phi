#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class deleteNodeCommand :
        public command
    {
    private:
        vector<node*> _nodesToDelete;
        map<node*, node*> _nodesParents;

    public:
        LAYERS_API deleteNodeCommand(const vector<node*>& nodesToDelete);
        LAYERS_API ~deleteNodeCommand();
        
        LAYERS_API void execute() override;
        LAYERS_API void executeUndo() override;
    };
}