#pragma once

#include <phi.h>
#include <core/boxCollider.h>
#include <core/obb.h>
#include <core/ray.h>
#include <core/transform.h>
#include "physicsApi.h"
#include "intersection/singleToSceneTest.h"
#include "intersection/groupToSceneTest.h"
#include "intersection/groupToGroupTest.h"
#include "intersection/singleToSingleTest.h"
#include "intersection/groupToSceneObbTest.h"
#include "sweep/singleToSceneTest.h"
#include "sweep/groupToSceneTest.h"
#include "sweep/groupToGroupTest.h"
#include "sweep/singleToSingleTest.h"
#include "sweep/singleToSceneObbTest.h"
#include "sweep/groupToSceneObbTest.h"
#include "sweep/sweepCollision.h"
#include "sweep/sweepTestResult.h"
#include "sweep/sweepObbCollision.h"
#include "sweep/sweepObbTestResult.h"
#include "rayCastResult.h"
#include "rayCastTest.h"

namespace phi
{
    class physicsWorld
    {
    public:
        PHYSICS_API physicsWorld();
        PHYSICS_API ~physicsWorld();

        PHYSICS_API void addCollider(boxCollider* collider);
        PHYSICS_API void removeCollider(boxCollider* collider);
        PHYSICS_API void changeTransform(boxCollider* collider, transform* newTransform);

        PHYSICS_API void setGroupOn(vector<boxCollider*>* colliders, uint16_t group);

        PHYSICS_API bool intersects(intersection::singleToSceneTest test);
        PHYSICS_API bool intersects(intersection::groupToSceneTest test);
        PHYSICS_API bool intersects(intersection::groupToGroupTest test);
        PHYSICS_API bool intersects(intersection::singleToSingleTest test);
        PHYSICS_API bool intersects(intersection::groupToSceneObbTest test);

        PHYSICS_API sweep::sweepTestResult sweep(sweep::singleToSceneTest test);
        PHYSICS_API sweep::sweepTestResult sweep(sweep::groupToSceneTest test);
        PHYSICS_API sweep::sweepTestResult sweep(sweep::singleToSingleTest test);
        PHYSICS_API sweep::sweepTestResult sweep(sweep::groupToGroupTest test);
        PHYSICS_API sweep::sweepObbTestResult sweep(sweep::singleToSceneObbTest test);
        PHYSICS_API sweep::sweepObbTestResult sweep(sweep::groupToSceneObbTest test);

        PHYSICS_API rayCastResult rayCast(const rayCastTest& test);

    private:
        static vec3 toVec3(const physx::PxVec3 vector);
        static physx::PxVec3 toPxVec3(const vec3 vector);
        static physx::PxTransform createPose(const obb& obb);
        static physx::PxTransform createPose(const boxCollider* collider, transform* transform);
        static physx::PxBoxGeometry createBoxGeometry(const vec3& halfSizes);
        static physx::PxBoxGeometry createBoxGeometry(const boxCollider* collider, transform* transform);

        void updateShape(boxCollider* collider);

        void enableQueryOn(boxCollider* collider);
        void enableQueryOn(vector<boxCollider*>* colliders);
        void disableQueryOn(boxCollider* collider);
        void disableQueryOn(vector<boxCollider*>* colliders);

        bool intersects(physx::PxBoxGeometry geometry, physx::PxTransform transform, uint16_t group);

    private:
        struct colliderData
        {
            transform* transform;
            eventToken transformChangedToken;
            eventToken isEnabledChangedToken;
            physx::PxRigidStatic* body;
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
    };
}