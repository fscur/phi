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
        selectionMouseController* _selectionMouseController;
    public:
        deleteObjectCommand(phi::scene* scene, selectionMouseController* selectionMouseController);
        ~deleteObjectCommand();
        void execute() override;
    };
}