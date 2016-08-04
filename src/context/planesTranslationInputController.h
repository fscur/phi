#pragma once

#include <phi.h>

#include "translationInputController.h"
#include "physicsLayerBehaviour.h"

namespace phi
{
    class planesTranslationInputController :
        public translationInputController
    {
    private:
        physicsLayerBehaviour* _physicsBehaviour;
        vector<translationPlane*> _translationPlanes;
        translationPlane* _lastChosenTranslationPlane;
        vec3 _lastCastPosition;

    private:
        void addTranslationPlane(translationPlane* translationPlane);
        void removeTranslationPlane(translationPlane* translationPlane);
        bool canTranslateAt(sweepCollision collision);
        vector<sweepCollision> findTouchCollisions();
        bool existsTranslationPlaneWithNormal(vec3 normal);
        void addPlanesIfNeeded(vector<sweepCollision> collisions);
        void removeDetachedPlanes(vector<sweepCollision> collisions);
        vec3 mouseOffsetToWorld(ivec2 mousePosition);
        translationPlane* findBestPlaneToDrag(vec3 dragDirection);
        void translateOn(translationPlane* translationPlane, ivec2 mousePosition);

    public:
        planesTranslationInputController(camera* camera, layer* planesLayer, physicsLayerBehaviour* physicsBehaviour);

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
    };
}