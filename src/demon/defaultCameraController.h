#pragma once
#include <phi.h>
#include <core\obb.h>
#include <rendering\planeGridRenderPass.h>
#include <scenes\boxCollider.h>
#include <scenes\scene.h>
#include <animation\floatAnimator.h>

#include "cameraController.h"
#include "selectionMouseController.h"

namespace demon
{
    struct contactTestResult
    {
        std::vector<phi::obb> contacts;
    };

    struct continuousCollisionResult
    {
        continuousCollisionResult(phi::obb obb, float time, phi::vec3 normal) :
            obb(obb),
            time(time),
            normal(normal)
        {
        }

        phi::obb obb;
        float time;
        phi::vec3 normal;
    };

    struct continuousContactTestResult
    {
        std::vector<continuousCollisionResult> contacts;
    };

    struct lineIntersectionResult
    {
        lineIntersectionResult()
        {
            point = phi::vec2();
            onLine1 = false;
            onLine2 = false;
            parallel = false;
        }

        phi::vec2 point;
        bool onLine1;
        bool onLine2;
        bool parallel;
    };

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
        selectionMouseController* _selectionMouseController;
        int32_t _mousePosX;
        int32_t _mousePosY;
        int32_t _lastMousePosX;
        int32_t _lastMousePosY;
        phi::image* _gridImage;
        phi::planeGridRenderPass* _planeGridPass;

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
        bool _dragDoingInertia;
        double _dragInertiaTime;
        phi::vec2 _dragStartPos;
        phi::vec2 _dragDelta;

    private:
        static phi::vec2 planePointProjection2D(phi::vec3 planeOrigin, phi::vec3 planeNormal, phi::vec3 planeRight, phi::vec3 planeUp, phi::vec3 point);
        static phi::aabb* getTransformedAabb(phi::node* node, phi::transform* transform);
        static phi::obb getTransformedObb(phi::boxCollider* collider, phi::transform* transform);
        lineIntersectionResult static lineIntersection(phi::vec2 line1Start, phi::vec2 line1End, phi::vec2 line2Start, phi::vec2 line2End);

        contactTestResult testContacts(phi::node* testNode, phi::obb obb);
        continuousContactTestResult testContinuousContacts(phi::node* testNode, phi::obb obb, phi::vec3 offset);

        void moveObject(phi::node* object, phi::vec3 offset, phi::vec3 planeNormal);

        void dragMouseDown(int mouseX, int mouseY);
        void dragMouseMove();
        void dragMouseUp();
        void dragUpdate();

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
        virtual ~defaultCameraController();

        virtual void update() override;

        selectionMouseController* getSelectionMouseController() { return _selectionMouseController; }
    };
}