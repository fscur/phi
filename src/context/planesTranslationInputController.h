#pragma once

#include <phi.h>

#include "translationInputController.h"
#include <core\clippingPlane.h>

namespace phi
{
    class planesTranslationInputController :
        public translationInputController
    {
    private:
        physicsLayerBehaviour* _physicsBehaviour;
        vector<translationPlane*> _translationPlanes;
        translationPlane* _lastChosenTranslationPlane;
        bool _isTouchingCollidedObject;
        bool _isSwitchingPlanes;
        ivec2 _switchPlanesMousePosition;
        bool _canChangePlanes;

    private:
        bool isDraggingObjectIntersectingAnyObject();
        void addTranslationPlane(translationPlane* translationPlane);
        void removeTranslationPlane(translationPlane* translationPlane);
        bool canTranslateAt(vec3 normal);
        vector<sweepCollision> findValidTouchCollisions();
        bool existsTranslationPlaneWithNormal(vec3 normal);
        void createClippingPlanes(translationPlane* clippingTranslationPlane, clippingDistance::clippingDistance clippingDistance);
        translationPlane* createTranslationPlane(plane plane, boxCollider* collidee, boxCollider* collider, clippingDistance::clippingDistance clippingDistance);
        void addPlanesIfNeeded(vector<sweepCollision> collisions);
        void removeDetachedPlanes(vector<sweepCollision> collisions);
        vec3 mouseOffsetToWorld(ivec2 mousePosition);
        translationPlane* findBestPlaneToDrag(vec3 dragDirection);
        bool isTouchingCollidedObject(vec3 offset, translationPlane* translationPlane, vec3& nearestPosition, plane& touchingPlane);
        vec3 checkForPossibleSwitchOfPlanes(vec3 offset, translationPlane* translationPlane, ivec2 mousePosition);
        void translateOn(translationPlane* translationPlane, ivec2 mousePosition);
        void changeToDefaultTranslationPlane();
        bool executeMouseMove(ivec2 mousePosition);

    public:
        planesTranslationInputController(camera* camera, layer* planesLayer, physicsLayerBehaviour* physicsBehaviour);

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
        virtual bool update() override;
    };
}