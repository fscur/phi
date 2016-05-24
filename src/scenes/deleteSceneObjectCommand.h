#pragma once
#include <phi.h>
#include "scenesApi.h"

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
        SCENES_API deleteSceneObjectCommand(const vector<node*>& nodesToDelete);
        SCENES_API ~deleteSceneObjectCommand();

        SCENES_API void execute() override;
        SCENES_API void executeUndo() override;
    };
}