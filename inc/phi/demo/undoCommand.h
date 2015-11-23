#ifndef _UNDO_COMMAND_H_
#define _UNDO_COMMAND_H_

#include "phi/demo/command.h"

class commandsManager;

class undoCommand :
    public command
{
private:
    commandsManager* _commandsManager;

protected:
    void execute() override;

public:
    undoCommand(commandsManager* commandsManager);
    bool getIsUndoable() { return false; }
};

#endif