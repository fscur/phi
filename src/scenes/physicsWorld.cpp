#include <precompiled.h>

#include <core\node.h>
#include "physicsWorld.h"

namespace phi
{
    physicsWorld::physicsWorld() :
        _colliders(unordered_map<boxCollider*, colliderData>())
    {
        _foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocatorCallback, _errorCallback);
        if (!_foundation)
            phi::debug("PxCreateFoundation failed!");

        _profileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(_foundation);
        if (!_profileZoneManager)
            phi::debug("PxProfileZoneManager::createProfileZoneManager failed!");

        _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), true, _profileZoneManager);
        if (!_physics)
            phi::debug("PxCreatePhysics failed!");

        physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
        _dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = _dispatcher;
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        _scene = _physics->createScene(sceneDesc);
    }

    physicsWorld::~physicsWorld()
    {
        while (_colliders.size() > 0u)
            removeCollider((*_colliders.begin()).first);

        _scene->release();
        _physics->release();
        _profileZoneManager->release();
        _foundation->release();
    }

    void physicsWorld::updateColliderPose(const boxCollider* collider, const colliderData data, transform* transform)
    {
        auto rotation = transform->getOrientation();
        auto center = rotation * collider->getPosition();

        auto modelMatrix = transform->getModelMatrix();
        auto pose = physx::PxTransform
        (
            physx::PxMat44
            (
                physx::PxVec4(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z, modelMatrix[0].w),
                physx::PxVec4(modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z, modelMatrix[1].w),
                physx::PxVec4(modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z, modelMatrix[2].w),
                physx::PxVec4(modelMatrix[3].x + center.x, modelMatrix[3].y + center.y, modelMatrix[3].z + center.z, modelMatrix[3].w)
            )
        );

        data.body->setGlobalPose(pose);
    }

    void physicsWorld::addCollider(boxCollider* collider)
    {
        auto material = _physics->createMaterial(0.0f, 0.0f, 0.0f);
        auto position = collider->getPosition();
        auto halfSizes = collider->getHalfSizes();

        colliderData data;
        auto pose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z));
        data.body = _physics->createRigidStatic(pose);
        data.geometry = new physx::PxBoxGeometry(glm::max(halfSizes.x, DECIMAL_TRUNCATION), glm::max(halfSizes.y, DECIMAL_TRUNCATION), glm::max(halfSizes.z, DECIMAL_TRUNCATION));
        data.body->createShape(*data.geometry, *material);
        data.body->userData = collider;

        data.transformChangedToken = collider->getNode()->getTransform()->getChangedEvent()->assign(
            [this, collider, data](transform* sender) -> void
        {
            updateColliderPose(collider, data, sender);
        });

        updateColliderPose(collider, data, collider->getNode()->getTransform());

        _scene->addActor(*data.body);

        _colliders[collider] = data;
    }

    void physicsWorld::removeCollider(boxCollider* collider)
    {
        auto data = _colliders[collider];
        collider->getNode()->getTransform()->getChangedEvent()->unassign(data.transformChangedToken);

        _scene->removeActor(*data.body);
        data.body->release();
        safeDelete(data.geometry);
        _colliders.erase(collider);
    }

    bool physicsWorld::intersects(
        boxCollider* collider,
        vector<boxCollider*> ignoreColliders)
    {
        auto geometry = _colliders[collider].geometry;
        auto pose = _colliders[collider].body->getGlobalPose();

        auto hit = physx::PxOverlapBuffer();
        auto filterCallback = new physicsWorld::filterCallback(ignoreColliders);
        if (_scene->overlap(*geometry, pose, hit, physx::PxQueryFilterData(physx::PxQueryFlag::eANY_HIT | physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC), filterCallback))
            return true;

        return false;
    }

    bool physicsWorld::intersects(
        const vector<boxCollider*>& colliders,
        vector<boxCollider*> ignoreColliders)
    {
        auto collidersCount = colliders.size();
        for (size_t i = 0; i < collidersCount; i++)
        {
            auto collider = colliders[i];
            auto result = intersects(collider, ignoreColliders);
            if (result)
                return true;
        }

        return false;
    }

    bool physicsWorld::intersects(
        boxCollider* collider,
        transform* const transform,
        vector<boxCollider*> ignoreColliders)
    {
        auto geometry = _colliders[collider].geometry;

        auto rotation = transform->getOrientation();
        auto center = rotation * collider->getPosition();
        auto position = center + transform->getPosition();
        auto pose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

        auto hit = physx::PxOverlapBuffer();
        auto filterCallback = physicsWorld::filterCallback(ignoreColliders);
        if (_scene->overlap(*geometry, pose, hit, physx::PxQueryFilterData(physx::PxQueryFlag::eANY_HIT | physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC), &filterCallback))
            return true;

        return false;
    }

    bool physicsWorld::intersects(
        const vector<boxCollider*>& colliders,
        const vector<transform*>& transforms,
        vector<boxCollider*> ignoreColliders)
    {
        auto collidersCount = colliders.size();
        for (size_t i = 0; i < collidersCount; i++)
        {
            auto collider = colliders[i];
            auto transform = transforms[i];

            auto result = intersects(collider, transform, ignoreColliders);

            if (result)
                return true;
        }

        return false;
    }

    sweepCollisionResult physicsWorld::intersectsSweep(
        boxCollider* collider,
        const vec3 offset,
        vector<boxCollider*> ignoreColliders)
    {
        auto result = sweepCollisionResult();

        auto geometry = _colliders[collider].geometry;
        auto pose = _colliders[collider].body->getGlobalPose();
        auto dir = glm::normalize(offset);
        auto distance = glm::length(offset);

        physx::PxSweepHit hitBuffer[32];
        auto hit = physx::PxSweepBuffer(hitBuffer, 32);
        auto filterCallback = physicsWorld::filterCallback(ignoreColliders);
        if (_scene->sweep(
            *geometry,
            pose,
            physx::PxVec3(dir.x, dir.y, dir.z),
            distance,
            hit,
            physx::PxHitFlag::eDEFAULT | physx::PxHitFlag::ePRECISE_SWEEP,
            physx::PxQueryFilterData(physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC),
            &filterCallback))
        {
            result.collided = true;

            auto touchesCount = hit.getNbTouches();
            for (size_t i = 0; i < touchesCount; i++)
            {
                auto touch = hit.getTouch(static_cast<physx::PxU32>(i));

                auto collider = reinterpret_cast<boxCollider*>(touch.actor->userData);
                result.collisions.push_back(sweepCollision(collider, touch.distance, vec3(touch.normal.x, touch.normal.y, touch.normal.z)));
            }
        }

        return result;
    }

    sweepCollisionResult physicsWorld::intersectsSweep(
        const vector<boxCollider*>& colliders,
        const vec3 offset,
        vector<boxCollider*> ignoreColliders)
    {
        auto allResult = sweepCollisionResult();

        auto collidersCount = colliders.size();
        for (size_t i = 0; i < collidersCount; i++)
        {
            auto collider = colliders[i];
            auto result = intersectsSweep(collider, offset, ignoreColliders);

            if (result.collided)
            {
                allResult.collided = true;

                auto collisionsCount = result.collisions;
                for (auto collision : result.collisions)
                {
                    auto alreadyFoundCollision = std::find_if(allResult.collisions.begin(), allResult.collisions.end(),
                        [&collision](sweepCollision const& sc)
                    {
                        return collision.collider == sc.collider;
                    });

                    if (alreadyFoundCollision != allResult.collisions.end())
                    {
                        if (collision.distance < alreadyFoundCollision->distance)
                        {
                            phi::removeIfContains(allResult.collisions, *alreadyFoundCollision);
                            allResult.collisions.push_back(collision);
                        }
                    }
                    else
                        allResult.collisions.push_back(collision);
                }
            }
        }

        std::sort(allResult.collisions.begin(), allResult.collisions.end(), [](const sweepCollision& a, const sweepCollision& b) -> bool
        {
            return a.distance < b.distance;
        });

        return allResult;
    }

    sweepCollisionResult physicsWorld::intersectsSweep(
        boxCollider* collider,
        transform* const transform,
        const vec3 offset,
        vector<boxCollider*> ignoreColliders)
    {
        auto result = sweepCollisionResult();

        auto geometry = _colliders[collider].geometry;
        auto dir = glm::normalize(offset);
        auto distance = glm::length(offset);

        auto rotation = transform->getOrientation();
        auto center = rotation * collider->getPosition();
        auto position = center + transform->getPosition();
        auto pose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

        physx::PxSweepHit hitBuffer[32];
        auto hit = physx::PxSweepBuffer(hitBuffer, 32);
        auto filterCallback = physicsWorld::filterCallback(ignoreColliders);
        if (_scene->sweep(
            *geometry,
            pose,
            physx::PxVec3(dir.x, dir.y, dir.z),
            distance,
            hit,
            physx::PxHitFlag::eDEFAULT | physx::PxHitFlag::ePRECISE_SWEEP,
            physx::PxQueryFilterData(physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC),
            &filterCallback))
        {
            result.collided = true;

            auto touchesCount = hit.getNbTouches();
            for (size_t i = 0; i < touchesCount; i++)
            {
                auto touch = hit.getTouch(static_cast<physx::PxU32>(i));

                auto collider = reinterpret_cast<boxCollider*>(touch.actor->userData);
                result.collisions.push_back(sweepCollision(collider, touch.distance, vec3(touch.normal.x, touch.normal.y, touch.normal.z)));
            }
        }

        return result;
    }

    sweepCollisionResult physicsWorld::intersectsSweep(
        const vector<boxCollider*>& colliders,
        const vector<transform*>& transforms,
        const vec3 offset,
        vector<boxCollider*> ignoreColliders)
    {
        auto allResult = sweepCollisionResult();

        auto collidersCount = colliders.size();
        for (size_t i = 0; i < collidersCount; i++)
        {
            auto collider = colliders[i];
            auto transform = transforms[i];

            auto result = intersectsSweep(collider, transform, offset, ignoreColliders);

            if (result.collided)
            {
                allResult.collided = true;

                auto collisionsCount = result.collisions;
                for (auto collision : result.collisions)
                {
                    auto alreadyFoundCollision = std::find_if(allResult.collisions.begin(), allResult.collisions.end(),
                        [&collision](sweepCollision const& sc)
                    {
                        return collision.collider == sc.collider;
                    });

                    if (alreadyFoundCollision != allResult.collisions.end())
                    {
                        if (collision.distance < alreadyFoundCollision->distance)
                        {
                            phi::removeIfContains(allResult.collisions, collision);
                            allResult.collisions.push_back(collision);
                        }
                    }
                    else
                        allResult.collisions.push_back(collision);
                }
            }
        }

        std::sort(allResult.collisions.begin(), allResult.collisions.end(), [](const sweepCollision& a, const sweepCollision& b) -> bool
        {
            return a.distance < b.distance;
        });

        return allResult;
    }
}