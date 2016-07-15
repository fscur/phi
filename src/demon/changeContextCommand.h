#pragma once

#include <core\command.h>

namespace demon
{
    bool _design = true;
    class changeContextCommand :
        public phi::command
    {
    public:
        virtual void execute() override { _design = !_design; }
        virtual void executeUndo() override { _design = !_design; }
    };
}