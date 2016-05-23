#pragma once
#include <phi.h>
#include <scenes\scene.h>
#include <animation\floatAnimator.h>
#include <apps\commandsManager.h>

#include "cameraController.h"

namespace demon
{
    class defaultCameraController :
        public cameraController
    {
    private:
        float const ZOOM_FACTOR = 1.0f;
        float const ZOOM_MIN_BOUNCE = 0.01f;
        float const ZOOM_MAX_BOUNCE = 0.1f;
        int32_t const ZOOM_ACCUMULATION_TIME = 500;

    private:
        phi::scene* _scene;
        phi::commandsManager* _commandsManager;
        int32_t _mousePosX;
        int32_t _mousePosY;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;
        phi::image* _gridImage;

        phi::vec3 _zoomDir;
        phi::vec3 _zoomCameraPos;
        float _zoomSpeed;
        float _zoomDistanceTraveled;
        float _zoomDistanceLimit;
        double _zoomSpeedAccumulationTime;
        double _zoomInertiaTime;
        phi::floatAnimation* _zoomBounceAnimation;

        bool _rotating;
        bool _rotationDoingInertia;
        phi::vec2 _rotationDelta;
        phi::vec3 _rotationTargetPos;
        double _rotationLastMouseMoveTime;
        double _rotationInertiaTime;
        float _rotationInertiaLastPercent;

        bool _panning;
        bool _panDoingInertia;
        float _panEyeZ;
        phi::vec3 _panCameraPos;
        phi::vec3 _panCameraRight;
        phi::vec3 _panCameraUp;
        glm::vec3 _panDelta;
        phi::vec3 _panTargetCameraPos;
        double _panLastMouseMoveTime;
        double _panInertiaTime;

        bool _dragging;
        phi::node* _dragObject;
        phi::vec3 _dragObjectStartPosition;
        phi::vec3 _dragOrigin;
        phi::vec3 _dragPlaneBottomLeft;
        phi::vec3 _dragPlaneBottomRight;
        phi::vec3 _dragPlaneTopRight;
        phi::vec3 _dragPlaneTopLeft;

    private:
        void dragMouseDown(int mouseX, int mouseY);
        void dragMouseMove();
        void dragMouseUp();

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
        defaultCameraController(phi::scene* scene, phi::commandsManager* commandsManager);
        virtual ~defaultCameraController();

        virtual void update() override;
    };
}