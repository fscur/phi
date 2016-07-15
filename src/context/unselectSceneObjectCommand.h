#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class unselectSceneObjectCommand :
        public command
    {
    private:
        vector<node*> _nodesToUnselect;
        vector<node*> _unselectedNodes;
    public:
        CONTEXT_API unselectSceneObjectCommand(const vector<node*>& nodesToUnselect);
        CONTEXT_API ~unselectSceneObjectCommand();

        CONTEXT_API virtual void execute();
        CONTEXT_API virtual void executeUndo();
    };
}