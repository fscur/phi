#pragma once

#include <phi.h>

#include <core\node.h>
#include <core\transform.h>
#include <core\boxCollider.h>
#include <scenes\physicsWorld.h>

namespace demon
{
    class planeDrag
    {
    private:
        phi::physicsWorld* _physicsWorld;
        phi::node* _currentObject;
        phi::vector<phi::boxCollider*> _currentColliders;
        phi::vector<phi::transform*> _currentTransforms;
        phi::plane _currentPlane;
        phi::vec3 _currentInitialPlanePosition;
        phi::vec3 _currentInitialObjectPosition;

    private:
        phi::vector<phi::transform*>* createOffsetTransforms(phi::vec3 offset);
        bool objectFitsInOffsetedPosition(phi::vec3 offset);
        phi::sweepCollisionResult* performCollisionSweep(phi::vector<phi::transform*>* transforms, phi::vec3 offset);
        phi::vector<phi::boxCollider*>* removeAndGetTouchingCollisions(phi::sweepCollisionResult* sweepResult, phi::vec3 offset);
        phi::sweepCollision findFarthestValidCollision(phi::sweepCollisionResult* sweepResult, phi::vec3 offset);
        phi::vec3 getAdjustedOffset(phi::sweepCollision collision, phi::vec3 offset);
        void moveObject(phi::vec3 offset);

    public:
        planeDrag(phi::physicsWorld* physicsWorld) :
            _physicsWorld(physicsWorld),
            _currentObject(nullptr),
            _currentColliders(),
            _currentTransforms(),
            _currentPlane(phi::vec3(), phi::vec3())
        {
        }

        void startDrag(phi::node* object, phi::plane plane);
        void updateDrag(phi::ray ray);
    };
}