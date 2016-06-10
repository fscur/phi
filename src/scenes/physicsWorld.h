#pragma once

#include <phi.h>
#include <core\boxCollider.h>
#include <core\obb.h>
#include <core\ray.h>

#include "scenesApi.h"

namespace phi
{
    struct sweepCollision
    {
        sweepCollision(boxCollider* collider, float distance, vec3 normal) :
            collider(collider), distance(distance), normal(normal)
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
            collided(false)
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
            filterCallback(vector<boxCollider*>& ignoreColliders) :
                ignoreColliders(ignoreColliders)
            {
            }

            vector<boxCollider*> ignoreColliders;

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
        unordered_map<boxCollider*, colliderData> _colliders;

        physx::PxFoundation* _foundation;
        physx::PxDefaultAllocator _allocatorCallback;
        physx::PxDefaultErrorCallback _errorCallback;
        physx::PxProfileZoneManager* _profileZoneManager;
        physx::PxPhysics* _physics;
        physx::PxScene* _scene;
        physx::PxDefaultCpuDispatcher* _dispatcher;

    private:
        void updateColliderPose(const boxCollider* collider, const colliderData data, transform* transform);

    public:
        SCENES_API physicsWorld();
        SCENES_API ~physicsWorld();

        void addCollider(boxCollider* collider);
        void removeCollider(boxCollider* collider);

        SCENES_API bool intersects(boxCollider* collider, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());
        SCENES_API bool intersects(const vector<boxCollider*>& colliders, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());

        SCENES_API bool intersects(boxCollider* collider, transform* const transform, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());
        SCENES_API bool intersects(const vector<boxCollider*>& colliders, const vector<transform*>& transforms, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());

        SCENES_API sweepCollisionResult intersectsSweep(boxCollider* collider, const vec3 offset, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());
        SCENES_API sweepCollisionResult intersectsSweep(const vector<boxCollider*>& colliders, const vec3 offset, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());

        SCENES_API sweepCollisionResult intersectsSweep(boxCollider* collider, transform* const transform, const vec3 offset, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());
        SCENES_API sweepCollisionResult intersectsSweep(const vector<boxCollider*>& colliders, const vector<transform*>& transforms, const vec3 offset, vector<boxCollider*> ignoreColliders = vector<boxCollider*>());
    };
}