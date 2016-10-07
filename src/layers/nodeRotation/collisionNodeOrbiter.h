#pragma once

#include <phi.h>
#include <core/node.h>
#include <physics/physicsWorld.h>

namespace phi
{
    class collisionNodeOrbiter
    {
    public:
        collisionNodeOrbiter(physicsWorld* physicsWorld);
        ~collisionNodeOrbiter();

        void addNode(node* node);
        void addRange(const vector<node*>& nodes);
        void clear();
        float orbit(float angle, plane plane);

    private:
        vector<transform*>* createRotatedTransforms(node* node, quat rotation);

    private:
        physicsWorld* _physicsWorld;
        unordered_map<node*, vector<boxCollider*>> _nodesColliders;
        unordered_map<node*, vector<transform*>> _nodesTransforms;
    };
}