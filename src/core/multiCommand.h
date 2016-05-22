#pragma once
#include <phi.h>
#include "coreApi.h"

#include "command.h"

namespace phi
{
    class multiCommand :
        public command
    {
    private:
        vector<command*> _commands;
    public:
        multiCommand(vector<command*>&& commands) :
            _commands(commands)
        { 
        }

        virtual ~multiCommand() 
        {
            for (auto command : _commands)
                safeDelete(command);
        }

        virtual void execute() 
        {
            for (auto command : _commands)
                command->execute();
        }

        virtual void executeUndo() 
        {
            for (auto it = _commands.rbegin(); it != _commands.rend(); ++it)
                (*it)->executeUndo();
        }
    };
}