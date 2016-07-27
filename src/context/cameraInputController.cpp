#include <precompiled.h>

#include "cameraInputController.h"

namespace phi
{
    cameraInputController::cameraInputController(camera* camera) :
        inputController::inputController(),
        _zoomController(camera),
        _rotationController(camera),
        _panController(camera)
    {
    }

    bool cameraInputController::onKeyDown(keyboardEventArgs * e)
    {
        if (_zoomController.onKeyDown(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onKeyDown(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onKeyDown(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onKeyUp(keyboardEventArgs * e)
    {
        if (_zoomController.onKeyUp(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onKeyUp(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onKeyUp(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onMouseDown(mouseEventArgs* e)
    {
        if (_zoomController.onMouseDown(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onMouseDown(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onMouseDown(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onMouseMove(mouseEventArgs* e)
    {
        if (_zoomController.onMouseMove(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onMouseMove(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onMouseMove(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onMouseUp(mouseEventArgs* e)
    {
        if (_zoomController.onMouseUp(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onMouseUp(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onMouseUp(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onBeginMouseWheel(mouseEventArgs* e)
    {
        if (_zoomController.onBeginMouseWheel(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onBeginMouseWheel(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onBeginMouseWheel(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onMouseWheel(mouseEventArgs* e)
    {
        if (_zoomController.onMouseWheel(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onMouseWheel(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onMouseWheel(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::onEndMouseWheel(mouseEventArgs* e)
    {
        if (_zoomController.onEndMouseWheel(e))
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.onEndMouseWheel(e))
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.onEndMouseWheel(e))
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }

    bool cameraInputController::update()
    {
        if (_zoomController.update())
        {
            _rotationController.cancelRotation();
            _panController.cancelPan();
            return true;
        }

        if (_rotationController.update())
        {
            _zoomController.cancelZoom();
            _panController.cancelPan();
            return true;
        }

        if (_panController.update())
        {
            _zoomController.cancelZoom();
            _rotationController.cancelRotation();
            return true;
        }

        return false;
    }
}