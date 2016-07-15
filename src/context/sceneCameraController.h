#pragma once
#include <phi.h>
#include "contextApi.h"

#include <animation\floatAnimator.h>

#include <rendering\camera.h>

#include "iMouseController.h"

namespace phi
{
    class sceneCameraController :
        public iMouseController
    {
    private:
        float const ZOOM_FACTOR = 1.0f;
        float const ZOOM_MIN_BOUNCE = 0.01f;
        float const ZOOM_MAX_BOUNCE = 0.1f;
        int32_t const ZOOM_ACCUMULATION_TIME = 500;

    private:
        camera* _camera;
        bool _isCtrlPressed;
        int32_t _mousePosX;
        int32_t _mousePosY;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;

        vec3 _zoomDir;
        vec3 _zoomCameraPos;
        float _zoomSpeed;
        float _zoomDistanceTraveled;
        float _zoomDistanceLimit;
        double _zoomSpeedAccumulationTime;
        double _zoomInertiaTime;
        floatAnimation* _zoomBounceAnimation;

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
        void zoomMouseWheel(int mouseX, int mouseY, float delta);
        void zoomUpdate();
        void zoomCancel();

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
        CONTEXT_API sceneCameraController(camera* camera);
        CONTEXT_API virtual ~sceneCameraController();

        virtual void update() override;
    };
}