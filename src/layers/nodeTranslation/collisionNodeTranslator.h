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

        vector<transform*> createOffsetedTransforms(vec3 offset);
        vector<boxCollider*>* getPiledUpColliders() { return &_piledUpColliders; }
        vector<sweepCollision>* getLastTranslationTouchingCollisions() { return _lastTranslationTouchingCollisions; }
        obb getNodeDestinationObb(node* node)
        {
            auto destinationPosition = _nodesDestinationPositions[node];
            auto currentPosition = node->getTransform()->getPosition();
            auto deltaToDestination = destinationPosition - currentPosition;

            auto obb = node->getObb();
            auto destinationObb = obb::obb(*obb);
            destinationObb.center += deltaToDestination;

            return destinationObb;
        }

        void setPlane(plane value) { _plane = value; }
        void addNode(node* node);
        void addRange(const vector<node*>& nodes);
        void clear();
        vec3 translate(vec3 offset);

        void disableCollisions() { _resolveCollisions = false; }
        void enableCollisions() { _resolveCollisions = true; }

    private:
        transform* createOffsetedTransform(node* node, vec3 offset);
        void addTouchingCollisions(sweepCollisionResult* sweepResult, sweepCollision compareCollision);
        bool objectFitsInOffsetedPosition(vec3 offset);
        sweepCollisionResult* performCollisionSweep(vector<transform*>& transforms, vec3 offset);
        vector<boxCollider*> getSweepCollisionResultCollidees(sweepCollisionResult* sweepResult);
        bool findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset, sweepCollision& farthestValidCollision);
        vec3 getAdjustedOffset(sweepCollision collision, vec3 offset);
        vec3 resolveCollisions(vec3 offset);
        void translateNodes(vec3 offset);

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