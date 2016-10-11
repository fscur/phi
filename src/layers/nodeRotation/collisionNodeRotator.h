#pragma once

#include <phi.h>
#include <core/node.h>
#include <physics/physicsWorld.h>

namespace phi
{
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

        void disableCollisions() { _resolveCollisions = false; }
        void enableCollisions() { _resolveCollisions = true; }

    private:
        vector<transform*>* createTransformedTransforms(node* node, vec3 offset, quat rotation);
        bool isValidOrbit(float angle, plane plane);
        bool isValidRotation(float angle, plane plane);
        float findMaximumOrbitPossible(float angle, plane plane);
        float findMaximumRotationPossible(float angle, plane plane);
        void orbitNodes(float angle, plane plane);
        void rotateNodes(float angle, plane plane);

    private:
        physicsWorld* _physicsWorld;
        unordered_map<node*, vector<boxCollider*>> _nodesColliders;
        unordered_map<node*, vector<transform*>> _nodesTransforms;
        bool _resolveCollisions;
    };
}