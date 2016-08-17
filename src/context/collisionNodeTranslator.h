#pragma once

#include <core\node.h>

#include <physics\physicsWorld.h>

namespace phi
{
    class collisionNodeTranslator
    {
    private:
        physicsWorld* _physicsWorld;
        plane _plane;
        node* _node;
        vector<boxCollider*> _colliders;
        vector<transform*> _transforms;
        vector<sweepCollision>* _lastTranslationTouchingCollisions;

    private:
        vector<sweepCollision>* getTouchingCollisions(sweepCollisionResult* sweepResult, sweepCollision compareCollision);
        vector<transform*>* createOffsetTransforms(vec3 offset);
        bool objectFitsInOffsetedPosition(vec3 offset);
        sweepCollisionResult* performCollisionSweep(vector<transform*>* transforms, vec3 offset, uint32_t maximumHits = 32u);
        vector<boxCollider*>* getSweepCollisionResultColliders(sweepCollisionResult* sweepResult);
        bool findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset, sweepCollision& farthestValidCollision);
        vec3 getAdjustedOffset(sweepCollision collision, vec3 offset);
        vec3 collisionNodeTranslator::getUndisruptedOffset(vec3 offset);

    public:
        collisionNodeTranslator(physicsWorld* physicsWorld);

        vector<boxCollider*>* getColliders() { return &_colliders; }
        vector<transform*>* getTransforms() { return &_transforms; }
        vector<sweepCollision>* getLastTranslationTouchingCollisions() { return _lastTranslationTouchingCollisions; }

        void setPlane(plane value) { _plane = value; }
        void setNode(node* node);

        void translateNode(vec3 offset);
    };
}