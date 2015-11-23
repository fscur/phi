#ifndef _MULTI_COMMAND_H_
#define _MULTI_COMMAND_H_

#include "phi/demo/command.h"

#include <vector>

class multiCommand :
    public command
{
private:
    std::vector<command*> _commands;

protected:
    void execute() override;
    void executeUndo() override;

public:
    multiCommand(std::vector<command*> commands);
};

#endif