#include "phi/demo/undoCommand.h"
#include "phi/demo/commandsManager.h"

undoCommand::undoCommand(commandsManager* commandsManager)
    : command()
{
    _commandsManager = commandsManager;
}

void undoCommand::execute()
{
    _commandsManager->undo();
}