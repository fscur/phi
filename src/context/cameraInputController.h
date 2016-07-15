#pragma once
#include <phi.h>
#include "contextApi.h"

#include <rendering\camera.h>

#include "inputController.h"

namespace phi
{
    class cameraInputController :
        public inputController
    {
    private:
        camera* _camera;
        bool _isCtrlPressed;
        int32_t _mousePosX;
        int32_t _mousePosY;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;

        bool _rotating;
        bool _rotationDoingInertia;
        vec2 _rotationDelta;
        vec3 _rotationTargetPos;
        double _rotationLastMouseMoveTime;
        double _rotationInertiaTime;
        float _rotationInertiaLastPercent;

        bool _panning;
        bool _panDoingInertia;
        float _panEyeZ;
        vec3 _panCameraPos;
        vec3 _panCameraRight;
        vec3 _panCameraUp;
        vec3 _panDelta;
        vec3 _panTargetCameraPos;
        double _panLastMouseMoveTime;
        double _panInertiaTime;

    private:
        void panMouseDown(int mouseX, int mouseY);
        void panMouseMove();
        void panMouseUp();
        void panUpdate();
        void panCancel();

        void rotationMouseDown(int mouseX, int mouseY);
        void rotationMouseMove();
        void rotationMouseUp();
        void rotationUpdate();
        void rotationCancel();

        virtual void onKeyDown(keyboardEventArgs* e) override;
        virtual void onKeyUp(keyboardEventArgs* e) override;

        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseWheel(mouseEventArgs* e) override;

    public:
        CONTEXT_API cameraInputController(camera* camera);
        CONTEXT_API virtual ~cameraInputController() {}

        virtual void update() override;
    };
}