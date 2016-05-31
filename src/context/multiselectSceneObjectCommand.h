#pragma once
#include <phi.h>
#include "scenesApi.h"

#include <core\command.h>
#include <core\node.h>

namespace phi
{
    class multiselectSceneObjectCommand :
        public command
    {
    private:
        node* _nodeToSelect;
    
    private:
        node * getSonOfRoot(node* node);
    public:
        SCENES_API multiselectSceneObjectCommand(node* nodeToSelect);
        SCENES_API ~multiselectSceneObjectCommand();


        SCENES_API virtual void execute();
        SCENES_API virtual void executeUndo();
    };
}