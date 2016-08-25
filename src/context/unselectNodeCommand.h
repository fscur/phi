#pragma once
#include <phi.h>
#include "contextApi.h"

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
        CONTEXT_API unselectNodeCommand(const vector<node*>& nodesToUnselect);
        CONTEXT_API ~unselectNodeCommand();

        CONTEXT_API virtual void execute();
        CONTEXT_API virtual void executeUndo();
    };
}