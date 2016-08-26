#pragma once
#include <phi.h>
#include "physicsApi.h"
#include <core\node.h>

#include <physics\physicsWorld.h>

namespace phi
{
    class physicsLayerBehaviour
    {
    private:
        physicsWorld* _physicsWorld;

    public:
        PHYSICS_API physicsLayerBehaviour();
        PHYSICS_API ~physicsLayerBehaviour();

        physicsWorld* getPhysicsWorld() { return _physicsWorld; }
        PHYSICS_API void onNodeAdded(node* node);
        PHYSICS_API void onNodeRemoved(node* node);
    };
}