#pragma once

#include <physics\physicsWorld.h>

#include "nodeTranslator.h"

namespace phi
{
    class collisionNodeTranslator :
        public nodeTranslator
    {
    private:
        physicsWorld* _physicsWorld;
        plane _plane;
        vector<boxCollider*> _colliders;
        vector<transform*> _transforms;

    private:
        vector<transform*>* createOffsetTransforms(vec3 offset);
        bool objectFitsInOffsetedPosition(vec3 offset);
        sweepCollisionResult* performCollisionSweep(vector<transform*>* transforms, vec3 offset);
        vector<boxCollider*>* getSweepCollisionResultColliders(sweepCollisionResult* sweepResult);
        sweepCollision findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset);
        vec3 getAdjustedOffset(sweepCollision collision, vec3 offset);

    public:
        collisionNodeTranslator(physicsWorld* physicsWorld);

        void setPlane(plane value) { _plane = value; }
        void setNode(node* node) override;

        void beginTranslations() override;
        void translateNode(vec3 offset) override;
        void endTranslations() override;
    };
}