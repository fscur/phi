#pragma once
#include <phi.h>

#include <scenes\scene.h>

#include <apps\command.h>

#include "selectionMouseController.h"

namespace demon
{
    class addObjectCommand :
        public phi::command
    {
    private:
        phi::scene* _scene;
        phi::node* const _objectToAdd;
    public:
        addObjectCommand(phi::scene* scene, phi::node* const objectToAdd);
        ~addObjectCommand();

        void execute() override;
        void executeUndo() override;
    };
}