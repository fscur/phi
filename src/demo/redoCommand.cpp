#include "phi/demo/redoCommand.h"
#include "phi/demo/commandsManager.h"

redoCommand::redoCommand(commandsManager* commandsManager)
    : command()
{
    _commandsManager = commandsManager;
}

void redoCommand::execute()
{
    _commandsManager->redo();
}