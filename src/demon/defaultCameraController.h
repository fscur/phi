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
        phi::scene* _scene;
        int _mousePosX;
        int _mousePosY;
        int _lastMousePosX;
        int _lastMousePosY;

        phi::vec3 _zoomDir;
        float _zoomSpeed;
        float _zoomDistanceTraveled;
        float _zoomDistanceLimit;
        unsigned int _zoomSpeedAccumulationTime;
        unsigned int _zoomInertiaTime;
        phi::floatAnimation* _zoomBounceAnimation;

        bool _rotating;
        bool _rotationDoingInertia;
        phi::vec2 _rotationDelta;
        phi::vec3 _rotationTargetPos;
        unsigned int _rotationLastMouseMoveTime;
        unsigned int _rotationInertiaTime;
        float _rotationInertiaLastPercent;

        bool _panning;
        bool _panDoingInertia;
        float _panEyeZ;
        phi::vec3 _panCameraPos;
        phi::vec3 _panCameraRight;
        phi::vec3 _panCameraUp;
        glm::vec3 _panDelta;
        phi::vec3 _panTargetCameraPos;
        unsigned int _panLastMouseMoveTime;
        unsigned int _panInertiaTime;

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