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

    private:
        bool canTranslateAt(sweepCollision collision);
        vector<sweepCollision> findTouchCollisions();
        bool existsTranslationPlaneWithNormal(vec3 normal);
        void addPlanesIfNeeded(vector<sweepCollision> collisions);
        void removeDetachedPlanes(vector<sweepCollision> collisions);

    public:
        planesTranslationInputController(camera* camera, layer* planesLayer, physicsLayerBehaviour* physicsBehaviour);

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;
    };
}