#pragma once

#include <phi.h>
#include <core\boxCollider.h>
#include <core\obb.h>
#include <core\ray.h>

#include "scenesApi.h"

namespace phi
{
    struct intersectionCollisionTest
    {
        intersectionCollisionTest() :
            collider(nullptr),
            transform(nullptr),
            shouldCollideWithItself(false),
            toIgnoreColliders(nullptr)
        {
        }

        boxCollider* collider;
        transform* transform;
        bool shouldCollideWithItself;
        vector<boxCollider*>* toIgnoreColliders;

        vector<boxCollider*>* getAllToIgnoreColliders()
        {
            vector<boxCollider*>* allToIgnoreColliders;
            if (toIgnoreColliders)
                allToIgnoreColliders = new vector<boxCollider*>(toIgnoreColliders->begin(), toIgnoreColliders->end());
            else
                allToIgnoreColliders = new vector<boxCollider*>();

            if (!shouldCollideWithItself)
                allToIgnoreColliders->push_back(collider);

            return allToIgnoreColliders;
        }
    };

    struct intersectionCollisionMultiTest
    {
        intersectionCollisionMultiTest() :
            colliders(),
            transforms(),
            shouldCollideWithItself(false),
            toIgnoreColliders(nullptr)
        {
        }

        vector<boxCollider*>* colliders;
        vector<transform*>* transforms;
        bool shouldCollideWithItself;
        vector<boxCollider*>* toIgnoreColliders;

        vector<boxCollider*>* getAllToIgnoreColliders()
        {
            vector<boxCollider*>* allToIgnoreColliders;
            if (toIgnoreColliders)
                allToIgnoreColliders = new vector<boxCollider*>(toIgnoreColliders->begin(), toIgnoreColliders->end());
            else
                allToIgnoreColliders = new vector<boxCollider*>();

            if (!shouldCollideWithItself)
                allToIgnoreColliders->insert(allToIgnoreColliders->end(), colliders->begin(), colliders->end());

            return allToIgnoreColliders;
        }
    };

    struct sweepCollisionTest :
        public intersectionCollisionTest
    {
        sweepCollisionTest() :
            intersectionCollisionTest(),
            distance(0.0f),
            direction(vec3())
        {
        }

        float distance;
        vec3 direction;
    };

    struct sweepCollisionMultiTest :
        public intersectionCollisionMultiTest
    {
        sweepCollisionMultiTest() :
            intersectionCollisionMultiTest(),
            distance(0.0f),
            direction(vec3()),
            findOnlyClosestPerTarget(false)
        {
        }

        float distance;
        vec3 direction;
        bool findOnlyClosestPerTarget;
    };

    struct sweepCollisionPairTest
    {
        sweepCollisionPairTest() :
            colliderSource(nullptr),
            transformSource(nullptr),
            offset(vec3()),
            colliderTarget(nullptr),
            transformTarget(nullptr)
        {
        }

        boxCollider* colliderSource;
        transform* transformSource;
        vec3 offset;
        boxCollider* colliderTarget;
        transform* transformTarget;
    };

    struct sweepCollision
    {
        sweepCollision() :
            collider(nullptr),
            distance(0.0f),
            normal(vec3())
        {
        }

        sweepCollision(boxCollider* collider, float distance, vec3 normal) :
            collider(collider),
            distance(distance),
            normal(normal)
        {
        }

        boxCollider* collider;
        float distance;
        vec3 normal;

        bool operator ==(const sweepCollision& b)
        {
            return
                this->distance == b.distance &&
                this->normal == b.normal &&
                this->collider == b.collider;
        }

        bool operator !=(const sweepCollision& b)
        {
            return !(*this == b);
        }
    };

    struct sweepCollisionResult
    {
        sweepCollisionResult() :
            collided(false),
            collisions()
        {
        }

        bool collided;
        vector<sweepCollision> collisions;
    };

    class physicsWorld
    {
    private:
        struct filterCallback : public physx::PxQueryFilterCallback
        {
            filterCallback(vector<boxCollider*>* ignoreColliders) :
                ignoreColliders(ignoreColliders)
            {
            }

            vector<boxCollider*>* ignoreColliders;

            virtual physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags)
            {
                auto targetCollider = reinterpret_cast<boxCollider*>(actor->userData);
                if (phi::contains(ignoreColliders, targetCollider))
                    return physx::PxQueryHitType::eNONE;

                return physx::PxQueryHitType::eTOUCH;
            }

            virtual physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit)
            {
                return physx::PxQueryHitType::eTOUCH;
            }
        };

        struct colliderData
        {
            eventToken transformChangedToken;
            physx::PxRigidStatic* body;
            physx::PxGeometry* geometry;
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
        static physx::PxTransform createPose(const boxCollider* collider, transform* transform);
        sweepCollisionResult sweepPenetration(sweepCollisionPairTest test);

    public:
        SCENES_API physicsWorld();
        SCENES_API ~physicsWorld();

        SCENES_API void addCollider(boxCollider* collider);
        SCENES_API void removeCollider(boxCollider* collider);

        SCENES_API bool intersects(intersectionCollisionTest test);
        SCENES_API bool intersects(intersectionCollisionMultiTest test);

        SCENES_API sweepCollisionResult sweep(sweepCollisionTest test);
        SCENES_API sweepCollisionResult sweep(sweepCollisionMultiTest test);
    };
}