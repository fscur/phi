#ifndef _ZOOM_TO_FIT_COMMAND_H_
#define _ZOOM_TO_FIT_COMMAND_H_

#include "phi/core/object3D.h"
#include "phi/scenes/camera.h"

#include "phi/demo/command.h"

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