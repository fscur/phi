#include "redoCommand.h"
#include "commandsManager.h"

redoCommand::redoCommand(commandsManager* commandsManager)
    : command()
{
    _commandsManager = commandsManager;
}

void redoCommand::execute()
{
    _commandsManager->redo();
}