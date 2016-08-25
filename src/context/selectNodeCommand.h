#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\command.h>
#include <core\node.h>

#include "unselectNodeCommand.h"

namespace phi
{
    class selectNodeCommand :
        public command
    {
    private:
        node* _nodeToSelect;

    public:
        CONTEXT_API selectNodeCommand(node* nodeToSelect);
        CONTEXT_API ~selectNodeCommand();

        CONTEXT_API virtual void execute();
        CONTEXT_API virtual void executeUndo();
    };
}