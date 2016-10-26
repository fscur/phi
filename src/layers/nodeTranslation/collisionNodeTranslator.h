#pragma once

#include <core/node.h>
#include <animation/animator.h>
#include <animation/translateAnimation.h>
#include <physics/physicsWorld.h>

namespace phi
{
    class collisionNodeTranslator
    {
    public:
        collisionNodeTranslator(physicsWorld* physicsWorld);
        ~collisionNodeTranslator();

        vector<transform*>* createOffsetedTransforms(vec3 offset);
        vector<boxCollider*>* getPiledUpColliders() { return &_piledUpColliders; }
        vector<sweepCollision>* getLastTranslationTouchingCollisions() { return _lastTranslationTouchingCollisions; }
        vec3 getNodeDestinationPosition(node* node) { return _nodesDestinationPositions[node]; }

        void setPlane(plane value) { _plane = value; }
        void addNode(node* node);
        void addRange(const vector<node*>& nodes);
        void clear();
        vec3 translate(vec3 offset);

        void disableCollisions() { _resolveCollisions = false; }
        void enableCollisions() { _resolveCollisions = true; }

    private:
        void addTouchingCollisions(sweepCollisionResult* sweepResult, sweepCollision compareCollision);
        bool objectFitsInOffsetedPosition(vec3 offset);
        sweepCollisionResult* performCollisionSweep(vector<transform*>* transforms, vec3 offset, uint32_t maximumHits = 32u);
        vector<boxCollider*>* getSweepCollisionResultCollidees(sweepCollisionResult* sweepResult);
        bool findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset, sweepCollision& farthestValidCollision);
        vec3 getAdjustedOffset(sweepCollision collision, vec3 offset);
        vec3 collisionNodeTranslator::resolveCollisions(vec3 offset);

    private:
        bool _resolveCollisions;
        physicsWorld* _physicsWorld;
        plane _plane;
        vector<node*> _nodes;
        vector<node*> _piledUpNodes;
        vector<transform*> _piledUpTransforms;
        vector<boxCollider*> _piledUpColliders;
        vector<sweepCollision>* _lastTranslationTouchingCollisions;
        unordered_map<node*, vec3> _nodesDestinationPositions;
        unordered_map<node*, translateAnimation*> _nodesTranslateAnimations;
    };
}