#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include <phi\core\mouseEventArgs.h>
#include <phi\core\keyboardEventArgs.h>
#include <phi\core\inputKey.h>
#include <phi\core\size.h>
#include <phi\rendering\camera.h>

class cameraController
{
protected:
    phi::camera* _camera;

public:
    cameraController(phi::camera* camera) : _camera(camera) {}

    virtual bool onMouseDown(phi::mouseEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onMouseMove(phi::mouseEventArgs* e) { return false; }
    virtual bool onMouseUp(phi::mouseEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onKeyDown(phi::keyboardEventArgs* e, phi::inputKey key) { return false; }
    virtual bool onKeyUp(phi::keyboardEventArgs* e, phi::inputKey key) { return false; }
    virtual void update() {}
};

#endif