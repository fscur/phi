#pragma once
#include <phi.h>
#include "scenesApi.h"

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
        SCENES_API groupSceneObjectsCommand(vector<node*>& nodesToGroup);
        SCENES_API ~groupSceneObjectsCommand();

        SCENES_API virtual void execute();
        SCENES_API virtual void executeUndo();
    };
}