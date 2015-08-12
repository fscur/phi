#include "undoCommand.h"
#include "commandsManager.h"

undoCommand::undoCommand(commandsManager* commandsManager)
    : command()
{
    _commandsManager = commandsManager;
}

void undoCommand::execute()
{
    _commandsManager->undo();
}