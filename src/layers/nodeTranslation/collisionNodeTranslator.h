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
        vector<node*> _nodes;
        vector<boxCollider*> _colliders;
        vector<transform*> _transforms;
        vector<sweepCollision>* _lastTranslationTouchingCollisions;

    private:
        void addTouchingCollisions(sweepCollisionResult* sweepResult, sweepCollision compareCollision);
        bool objectFitsInOffsetedPosition(vec3 offset);
        sweepCollisionResult* performCollisionSweep(vector<transform*>* transforms, vec3 offset, uint32_t maximumHits = 32u);
        vector<boxCollider*>* getSweepCollisionResultCollidees(sweepCollisionResult* sweepResult);
        bool findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset, sweepCollision& farthestValidCollision);
        vec3 getAdjustedOffset(sweepCollision collision, vec3 offset);
        vec3 collisionNodeTranslator::getUndisruptedOffset(vec3 offset);


    public:
        collisionNodeTranslator(physicsWorld* physicsWorld);
        ~collisionNodeTranslator();

        vector<transform*>* createOffsetedTransforms(vec3 offset);

        vector<boxCollider*>* getColliders() { return &_colliders; }
        vector<transform*>* getTransforms() { return &_transforms; }

        vector<sweepCollision>* getLastTranslationTouchingCollisions() { return _lastTranslationTouchingCollisions; }

        void setPlane(plane value) { _plane = value; }
        void addNode(node* node);
        void addRange(const vector<node*>& nodes);
        void clear();
        void translate(vec3 offset);
    };
}