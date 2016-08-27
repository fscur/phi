#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class groupNodesCommand :
        public command
    {
    private:
        vector<node*> _nodesToGroup;
        map<node*, node*> _nodesParents;
        node* _group;

    public:
        LAYERS_API groupNodesCommand(vector<node*>& nodesToGroup);
        LAYERS_API ~groupNodesCommand();

        LAYERS_API virtual void execute();
        LAYERS_API virtual void executeUndo();
    };
}