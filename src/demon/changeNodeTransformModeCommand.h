#pragma once

#include <core\command.h>

namespace demon
{
    class changeNodeTransformModeCommand :
        public phi::command
    {
    public:
        virtual bool getIsUndoable() { return false; }
        virtual void execute() override { phi::debug("trocou"); }
        virtual void executeUndo() override { phi::debug("destrocou"); }
    };
}