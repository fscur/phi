#pragma once
#include <phi.h>
#include <scenes\scene.h>
#include <ui\floatAnimator.h>

#include "cameraController.h"

namespace demon
{
    class defaultCameraController :
        public cameraController
    {
    private:
        float const ZOOM_MIN_BOUNCE = 0.01f;
        float const ZOOM_MAX_BOUNCE = 0.25f;
        float const ZOOM_SUM_TIME = 0.5f;

    private:
        int _lastMousePosX;
        int _lastMousePosY;
        bool _rotating;
        bool _panning;
        GLfloat _zBufferValue;
        phi::vec3 _cameraPos;
        phi::vec3 _cameraRight;
        phi::vec3 _cameraUp;
        int _startPosX;
        int _startPosY;
        float _eyeZ;
        phi::vec3 _targetPos;
        phi::scene* _scene;

        phi::vec3 _zoomDir;
        float _zoomSpeed;
        float _zoomTime;
        float _zoomSumTime;
        float _zoomCurrent;
        float _zoomLimit;
        bool _zoomResetSpeed;
        float _zoomBounceValue;
        phi::floatAnimation* _zoomBounceAnimation;
        float _zoomZ;
        float _zoomZNear;

    private:
        void initPan(int mouseX, int mouseY);
        void initRotate(int mouseX, int mouseY);
        void zoom(int mouseX, int mouseY, float delta);
        void pan(int mouseX, int mouseY);
        void rotate(int mouseX, int mouseY);

        virtual void onMouseDown(phi::mouseEventArgs* e) override;
        virtual void onMouseMove(phi::mouseEventArgs* e) override;
        virtual void onMouseUp(phi::mouseEventArgs* e) override;
        virtual void onMouseWheel(phi::mouseEventArgs* e) override;

    public:
        defaultCameraController(phi::scene* scene);
        virtual ~defaultCameraController() {}

        virtual void update() override;
    };
}