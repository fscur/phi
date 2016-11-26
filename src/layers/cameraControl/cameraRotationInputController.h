#pragma once

#include <phi.h>

#include <rendering\camera.h>

#include <input\inputController.h>

namespace phi
{
    class cameraRotationInputController :
        public inputController
    {
    public:
        cameraRotationInputController(camera* camera);

        void cancelRotation();
        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool update() override;

    private:
        void orbitCamera(float horizontalAngle, float verticalAngle);

    private:
        camera* _camera;
        bool _rotating;
        bool _doingInertia;
        vec2 _delta;
        vec3 _origin;
        double _lastMouseMoveTime;
        double _inertiaTime;
        float _inertiaLastPercent;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;
    };
}