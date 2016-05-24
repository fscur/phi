#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
    class command
    {
    public:
        command() { };
        virtual ~command() { };
        virtual bool getIsUndoable() { return true; }

        virtual void execute() { };
        virtual void executeUndo() { };
    };
}