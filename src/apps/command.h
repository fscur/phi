#pragma once
#include <phi.h>
#include "appsApi.h"

namespace phi
{
    class command
    {
    public:
        command() {};
        virtual bool getIsUndoable() { return true; }

        virtual void execute() { };
        virtual void executeUndo() { };
    };
}