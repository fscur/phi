#include "phi/demo/multiCommand.h"

multiCommand::multiCommand(std::vector<command*> commands)
    : command()
{
    _commands = commands;
}

void multiCommand::execute()
{
    auto commandsCount = _commands.size();
    for (unsigned int i = 0; i < commandsCount; i++)
    {
        auto cmd = _commands[i];
        cmd->start();
    }
}

void multiCommand::executeUndo()
{
    auto commandsCount = _commands.size();
    for (auto i = commandsCount - 1; i >= 0; i--)
    {
        auto cmd = _commands[i];
        cmd->startUndo();
    }
}