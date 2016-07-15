#pragma once

#include <phi.h>

#include <rendering\camera.h>

#include "inputController.h"

namespace phi
{
    class rotationInputController :
        public inputController
    {
    private:
        camera* _camera;
        bool _rotating;
        bool _doingInertia;
        vec2 _delta;
        vec3 _targetPos;
        double _lastMouseMoveTime;
        double _inertiaTime;
        float _inertiaLastPercent;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;

    public:
        rotationInputController(camera* camera);

        void cancelRotation();
        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool update() override;
    };
}