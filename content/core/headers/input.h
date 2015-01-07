#ifndef _PHI_INPUT_H_
#define _PHI_INPUT_H_

#include "core.h"
#include "mouseEventHandler.h"

namespace phi
{
    class input
    {
    public:
        CORE_API static void notifyMouseMove(mouseEventArgs e);
        CORE_API static void notifyMouseDown(mouseEventArgs e);
        CORE_API static void notifyMouseUp(mouseEventArgs e);
        CORE_API static void notifyMouseWheel(mouseEventArgs e);

        CORE_API static mouseEventHandler* mouseMove;
        CORE_API static mouseEventHandler* mouseDown;
        CORE_API static mouseEventHandler* mouseUp;
        CORE_API static mouseEventHandler* mouseWheel;
    };

}

#endif