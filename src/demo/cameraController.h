#pragma once
#include <precompiled.h>
#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>
#include <core\inputKey.h>
#include <core\size.h>
#include <rendering\camera.h>

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