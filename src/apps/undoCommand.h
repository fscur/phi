#pragma once

#include "commandsManager.h"

namespace phi
{
    class undoCommand :
        public command
    {
    private:
        commandsManager* _commandsManager;

    public:
        undoCommand(commandsManager* commandsManager)
            : command()
        {
            _commandsManager = commandsManager;
        }

        void undoCommand::execute()
        {
            _commandsManager->undo();
        }

        bool getIsUndoable() override { return false; }
    };
}