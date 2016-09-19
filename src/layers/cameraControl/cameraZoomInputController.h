#pragma once

#include <phi.h>

#include <rendering/camera.h>
#include <input/inputController.h>
#include <animation/positionAnimation.h>

namespace phi
{
    class cameraZoomInputController :
        public inputController
    {
    public:
        cameraZoomInputController(camera* camera);
        ~cameraZoomInputController();

        void cancelZoom();
        virtual bool onMouseWheel(mouseEventArgs* e) override;
        virtual bool update() override;

    private:
        void cancelBounceAnimation();
        void startBounceAnimation(float distance);

    private:
        float const ZOOM_FACTOR = 1.0f;
        float const MIN_BOUNCE = 0.01f;
        float const MAX_BOUNCE = 0.1f;
        int32_t const ACCUMULATION_TIME = 500;
        float const Z_NEAR_AVOIDING_GAP = 0.0001f;

        camera* _camera;
        vec3 _direction;
        vec3 _startingCameraPosition;
        float _speed;
        float _distanceTraveled;
        float _distanceLimit;
        float _bounceDistance;
        double _speedAccumulationTime;
        double _inertiaTime;
        positionAnimation* _bounceAnimation;
    };
}