#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class groupSceneObjectsCommand :
        public command
    {
    private:
        vector<node*> _nodesToGroup;
        map<node*, node*> _nodesParents;
        node* _group;

    public:
        CONTEXT_API groupSceneObjectsCommand(vector<node*>& nodesToGroup);
        CONTEXT_API ~groupSceneObjectsCommand();

        CONTEXT_API virtual void execute();
        CONTEXT_API virtual void executeUndo();
    };
}