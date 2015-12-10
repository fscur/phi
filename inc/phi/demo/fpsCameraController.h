#ifndef _FPS_CAMERA_CONTROLLER_H_
#define _FPS_CAMERA_CONTROLLER_H_

#include "cameraController.h"
#include "shortcuts.h"

#include <phi/core/globals.h>

class fpsCameraController :
    public cameraController
{
private:
    shortcuts _shortcuts;
    phi::vec2 _lastMousePos;
    phi::inputKey* _walkForward;
    phi::inputKey* _walkBackward;
    phi::inputKey* _walkLeft;
    phi::inputKey* _walkRight;

public:
    fpsCameraController(phi::camera* camera);

    virtual bool onMouseDown(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onMouseMove(phi::mouseEventArgs* e) override;
    virtual bool onMouseUp(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onKeyDown(phi::keyboardEventArgs* e, phi::inputKey key) override;

    virtual void update() override;
};

#endif