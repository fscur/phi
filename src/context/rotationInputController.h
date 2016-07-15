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

    private:
        void cancelRotation();

    public:
        rotationInputController(camera* camera);

        void onMouseDown(mouseEventArgs* e) override;
        void onMouseMove(mouseEventArgs* e) override;
        void onMouseUp(mouseEventArgs* e) override;
        void update() override;
    };
}