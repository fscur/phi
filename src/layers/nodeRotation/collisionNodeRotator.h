#pragma once

#include <phi.h>
#include <core/node.h>
#include <physics/physicsWorld.h>

namespace phi
{
    typedef unordered_map<node*, vector<transform*>> nodeTransforms;

    class collisionNodeRotator
    {
    public:
        collisionNodeRotator(physicsWorld* physicsWorld);
        ~collisionNodeRotator();

        void addNode(node* node);
        void addRange(const vector<node*>& nodes);
        void clear();
        float orbit(float angle, plane plane);
        float rotate(float angle, plane plane);

        void enableCollisions() { _resolveCollisions = true; }
        void disableCollisions() { _resolveCollisions = false; }
        void enableSelfCollision() { _isSelfCollisionEnabled = true; }
        void disableSelfCollision() { _isSelfCollisionEnabled = false; }

    private:
        vector<transform*>* createTransformedTransforms(node* node, vec3 offset, quat rotation);
        bool isValidOrbit(float angle, plane plane);
        bool isValidRotation(float angle, plane plane);
        float findMaximumOrbitPossible(float angle, plane plane);
        float findMaximumRotationPossible(float angle, plane plane);
        void orbitNodes(float angle, plane plane);
        void rotateNodes(float angle, plane plane);
        bool testIntersections(nodeTransforms transforms);
        nodeTransforms assignRotatedTransformsToPhysicsWorld(quat rotation);
        void assignOriginalTransformsToPhysicsWorld(nodeTransforms transforms);

    private:
        physicsWorld* _physicsWorld;
        unordered_map<node*, vector<boxCollider*>> _nodesColliders;
        unordered_map<node*, vector<transform*>> _nodesTransforms;
        bool _isSelfCollisionEnabled;
        bool _resolveCollisions;
    };
}