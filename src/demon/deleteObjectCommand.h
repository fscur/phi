#pragma once
#include <phi.h>

#include <scenes\scene.h>

#include <apps\command.h>

#include "selectionMouseController.h"

namespace demon
{
    class deleteObjectCommand :
        public phi::command
    {
    private:
        phi::scene* _scene;
        selectionMouseController* _selectionController;
        phi::map<phi::node*, phi::node*> _nodesParents;

    public:
        deleteObjectCommand(phi::scene* scene, selectionMouseController* selectionController);
        ~deleteObjectCommand();

        void execute() override;
        void executeUndo() override;
    };
}