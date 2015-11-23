#ifndef _REDO_COMMAND_H_
#define _REDO_COMMAND_H_

#include "phi/demo/command.h"

class commandsManager;

class redoCommand :
    public command
{
private:
    commandsManager* _commandsManager;

protected:
    void execute() override;

public:
    redoCommand(commandsManager* commandsManager);
    bool getIsUndoable() { return false; }
};

#endif