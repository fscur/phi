#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class deleteSceneObjectCommand :
        public command
    {
    private:
        vector<node*> _nodesToDelete;
        map<node*, node*> _nodesParents;

    public:
        CONTEXT_API deleteSceneObjectCommand(const vector<node*>& nodesToDelete);
        CONTEXT_API ~deleteSceneObjectCommand();

        CONTEXT_API void execute() override;
        CONTEXT_API void executeUndo() override;
    };
}