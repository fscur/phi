#pragma once

#include <phi.h>

#include <rendering\camera.h>

#include "inputController.h"

namespace phi
{
    class cameraPanInputController :
        public inputController
    {
    private:
        camera* _camera;
        bool _panning;
        bool _doingInertia;
        float _eyeZ;
        vec3 _cameraPos;
        vec3 _cameraRight;
        vec3 _cameraUp;
        vec3 _delta;
        vec3 _targetCameraPos;
        double _lastMouseMoveTime;
        double _inertiaTime;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;

    public:
        cameraPanInputController(camera* camera);

        void cancelPan();
        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool update();
    };
}