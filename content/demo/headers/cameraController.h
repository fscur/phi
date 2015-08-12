#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"
#include "inputKey.h"
#include "size.h"

class cameraController
{
protected:
    phi::size<unsigned int> _viewportSize;

public:
    cameraController(phi::size<unsigned int> viewportSize) : _viewportSize(viewportSize) {}

    virtual bool onMouseDown(phi::mouseEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onMouseMove(phi::mouseEventArgs* e) { return false; }
    virtual bool onMouseUp(phi::mouseEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onKeyDown(phi::keyboardEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onKeyUp(phi::keyboardEventArgs* e, phi::inputKey key) { return false; }
    virtual void update() {}
};

#endif