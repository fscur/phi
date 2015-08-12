#ifndef _MULTI_COMMAND_H_
#define _MULTI_COMMAND_H_

#include <vector>
#include "command.h"

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