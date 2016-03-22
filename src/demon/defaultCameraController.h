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
        const float ZOOM_SPEED_INCREASE = 1.0f;
        const float MAX_ZOOM_SPEED = 0.3f;
        const float ZOOM_SPEED_DECELERATION = 3.0f;

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

        phi::vec3 _zoomCameraStartPos;
        phi::vec3 _zoomDir;
        float _currentZoom;
        float _targetZoom;
        phi::floatAnimation* _zoomAnimation;
        float _zoomTime;
        int _zoomCount;
        float _zoomLimit;

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