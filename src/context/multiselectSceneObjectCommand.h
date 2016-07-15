#pragma once
#include <phi.h>
#include "contextApi.h"

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
        CONTEXT_API multiselectSceneObjectCommand(node* nodeToSelect);
        CONTEXT_API ~multiselectSceneObjectCommand();


        CONTEXT_API virtual void execute();
        CONTEXT_API virtual void executeUndo();
    };
}