#pragma once

#include <phi.h>
#include <core\boxCollider.h>
#include <core\obb.h>
#include <core\ray.h>

#include "physicsApi.h"
#include "intersectionCollisionTest.h"
#include "intersectionCollisionMultiTest.h"
#include "intersectionCollisionGroupTest.h"
#include "sweepCollisionTest.h"
#include "sweepCollisionMultiTest.h"
#include "sweepCollisionGroupTest.h"
#include "sweepCollisionPairTest.h"
#include "sweepCollision.h"
#include "sweepCollisionResult.h"
#include "rayCastResult.h"
#include "rayCastTest.h"

namespace phi
{
    class physicsWorld
    {
    private:
        struct colliderData
        {
            eventToken transformChangedToken;
            physx::PxRigidStatic* body;
            physx::PxGeometry* geometry;
            physx::PxShape* shape;
        };

    private:
        unordered_map<boxCollider*, colliderData*> _colliders;

        physx::PxFoundation* _foundation;
        physx::PxDefaultAllocator _allocatorCallback;
        physx::PxDefaultErrorCallback _errorCallback;
        physx::PxProfileZoneManager* _profileZoneManager;
        physx::PxPhysics* _physics;
        physx::PxScene* _scene;
        physx::PxDefaultCpuDispatcher* _dispatcher;

    private:
        static physx::PxVec3 toPxVec3(const vec3 vector);
        static physx::PxTransform createPose(const obb& obb);
        static physx::PxTransform createPose(const boxCollider* collider, transform* transform);
        static physx::PxBoxGeometry createBoxGeometry(const vec3& halfSizes);
        static physx::PxBoxGeometry createBoxGeometry(const boxCollider* collider, transform* transform);

    public:
        PHYSICS_API physicsWorld();
        PHYSICS_API ~physicsWorld();

        PHYSICS_API void addCollider(boxCollider* collider);
        PHYSICS_API void removeCollider(boxCollider* collider);

        PHYSICS_API void enableQueryOn(vector<boxCollider*>* colliders);
        PHYSICS_API void disableQueryOn(vector<boxCollider*>* colliders);

        PHYSICS_API void setGroupOn(vector<boxCollider*>* colliders, uint16_t group);

        PHYSICS_API bool intersects(intersectionCollisionTest test);
        PHYSICS_API bool intersects(intersectionCollisionMultiTest test);
        PHYSICS_API bool intersects(intersectionCollisionGroupTest test);

        PHYSICS_API sweepCollisionResult sweep(sweepCollisionPairTest test);
        PHYSICS_API sweepCollisionResult sweep(sweepCollisionTest test);
        PHYSICS_API sweepCollisionResult sweep(sweepCollisionMultiTest test);
        PHYSICS_API sweepCollisionResult sweep(sweepCollisionGroupTest test);

        PHYSICS_API rayCastResult rayCast(const rayCastTest& test);
    };
}