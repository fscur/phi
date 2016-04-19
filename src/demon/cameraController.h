#pragma once
#include <phi.h>

#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>
#include <core\input.h>

#include <rendering\camera.h>

namespace demon
{
    class cameraController
    {
    protected:
        phi::camera* _camera;

    public:
        cameraController(phi::camera* camera) :
            _camera(camera)
        {
            phi::input::mouseDown->assign(std::bind(&cameraController::onMouseDown, this, std::placeholders::_1));
            phi::input::mouseMove->assign(std::bind(&cameraController::onMouseMove, this, std::placeholders::_1));
            phi::input::mouseUp->assign(std::bind(&cameraController::onMouseUp, this, std::placeholders::_1));
            phi::input::mouseWheel->assign(std::bind(&cameraController::onMouseWheel, this, std::placeholders::_1));
            phi::input::keyDown->assign(std::bind(&cameraController::onKeyDown, this, std::placeholders::_1));
            phi::input::keyUp->assign(std::bind(&cameraController::onKeyUp, this, std::placeholders::_1));
        }

        virtual void onMouseDown(phi::mouseEventArgs* e) { }
        virtual void onMouseMove(phi::mouseEventArgs* e) { }
        virtual void onMouseUp(phi::mouseEventArgs* e) { }
        virtual void onMouseWheel(phi::mouseEventArgs* e) { }
        virtual void onKeyDown(phi::keyboardEventArgs* e) { }
        virtual void onKeyUp(phi::keyboardEventArgs* e) { }
        virtual void update() {}
    };
}