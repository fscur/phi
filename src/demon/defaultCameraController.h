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
        float const ZOOM_FACTOR = 0.03f;
        float const ZOOM_MIN_BOUNCE = 0.01f;
        float const ZOOM_MAX_BOUNCE = 0.1f;
        unsigned int const ZOOM_ACCUMULATION_TIME = 500u;

    private:
        int _mousePosX;
        int _mousePosY;
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
        phi::node* _cube;

        phi::vec3 _zoomDir;
        float _zoomSpeed;
        unsigned int _zoomSpeedAccumulationTime;
        unsigned int _zoomInertiaTime;
        float _zoomDistanceTraveled;
        float _zoomDistanceLimit;
        phi::floatAnimation* _zoomBounceAnimation;

        phi::vec2 _rotationSpeed;
        unsigned int _rotationInertiaTime;
        unsigned int _rotationLastMouseMoveTime;

        float _panSpeed;
        glm::vec3 _panDir;
        unsigned int _panInertiaTime;
        phi::floatAnimation* _panAnimation;
        phi::vec3 _virtualCameraPos;

    private:
        void initPan(int mouseX, int mouseY);
        void initRotate(int mouseX, int mouseY);

        void mouseWheelZoom(int mouseX, int mouseY, float delta);

        void mouseMovePan();
        void mouseMoveRotate();

        void updateZoom();
        void updateRotation();
        void updatePan();

        void cancelZoom();
        void cancelRotation();
        void cancelPan();

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