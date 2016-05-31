#pragma once
#include <phi.h>
#include "scenesApi.h"

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
        SCENES_API unselectSceneObjectCommand(const vector<node*>& nodesToUnselect);
        SCENES_API ~unselectSceneObjectCommand();

        SCENES_API virtual void execute();
        SCENES_API virtual void executeUndo();
    };
}