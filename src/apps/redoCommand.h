#pragma once

#include "commandsManager.h"

namespace phi
{
    class redoCommand :
        public command
    {
    private:
        commandsManager* _commandsManager;

    public:
        redoCommand(commandsManager* commandsManager)
            : command()
        {
            _commandsManager = commandsManager;
        }

        void redoCommand::execute()
        {
            _commandsManager->redo();
        }

        bool getIsUndoable() override { return false; }
    };
}