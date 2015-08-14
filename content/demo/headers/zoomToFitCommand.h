#ifndef _ZOOM_TO_FIT_COMMAND_H_
#define _ZOOM_TO_FIT_COMMAND_H_

#include "command.h"
#include "object3D.h"
#include "camera.h"

class zoomToFitCommand :
    public command
{
protected:
    void execute() override;

public:
    zoomToFitCommand();
    bool getIsUndoable() override { return false; }
};

#endif