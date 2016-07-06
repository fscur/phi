#pragma once

#include <phi.h>

#include <core\node.h>
#include <core\transform.h>
#include <core\boxCollider.h>
#include <scenes\scene.h>

namespace demon
{
    class mouseDrag
    {
    protected:
        phi::scene* _scene;
        phi::physicsWorld* _physicsWorld;
        phi::node* _object;
        phi::vector<phi::boxCollider*> _colliders;
        phi::vector<phi::transform*> _transforms;
        phi::plane _plane;
        phi::vec3 _initialPlanePosition;
        phi::vec3 _initialObjectPosition;

    protected:
        phi::vector<phi::transform*>* createOffsetTransforms(phi::vec3 offset);
        bool objectFitsInOffsetedPosition(phi::vec3 offset);
        phi::sweepCollisionResult* performCollisionSweep(phi::vector<phi::transform*>* transforms, phi::vec3 offset);
        phi::vector<phi::boxCollider*>* getSweepCollisionResultColliders(phi::sweepCollisionResult* sweepResult);
        phi::sweepCollision findFarthestValidCollision(phi::sweepCollisionResult* sweepResult, phi::vec3 offset);
        phi::vec3 getAdjustedOffset(phi::sweepCollision collision, phi::vec3 offset);
        void moveObject(phi::vec3 offset);

    public:
        mouseDrag(phi::scene* scene) :
            _scene(scene),
            _physicsWorld(_scene->getPhysicsWorld()),
            _object(nullptr),
            _colliders(),
            _transforms(),
            _plane(phi::vec3(), phi::vec3())
        {
        }

        virtual ~mouseDrag() {}

        virtual void startDrag(int mouseX, int mouseY);
        virtual void drag(int mouseX, int mouseY) {}
        virtual void update() {}
        virtual void endDrag();
    };
}