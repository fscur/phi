#pragma once

#include <phi.h>

#include <core\node.h>

#include <physics\physicsWorld.h>

namespace phi
{
    class physicsLayerBehaviour
    {
    private:
        physicsWorld* _physicsWorld;

    public:
        physicsLayerBehaviour() :
            _physicsWorld(new physicsWorld())
        {
        }

        ~physicsLayerBehaviour()
        {
            safeDelete(_physicsWorld);
        }

        physicsWorld* getPhysicsWorld() { return _physicsWorld; }
        void onNodeAdded(node* node);
        void onNodeRemoved(node* node);
    };
}