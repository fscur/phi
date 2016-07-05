#include <precompiled.h>

#include <core\node.h>
#include "physicsWorld.h"

namespace phi
{
    physicsWorld::physicsWorld() :
        _colliders(unordered_map<boxCollider*, colliderData*>())
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
        {
            auto pair = *_colliders.begin();
            removeCollider(pair.first);
            safeDelete(pair.second);
        }

        _scene->release();
        _physics->release();
        _profileZoneManager->release();
        _foundation->release();
    }

    physx::PxTransform physicsWorld::createPose(const boxCollider* collider, transform* transform)
    {
        auto rotation = transform->getOrientation();
        auto center = rotation * collider->getPosition();
        auto position = center + transform->getPosition();

        return physx::PxTransform(
            physx::PxVec3(position.x, position.y, position.z),
            physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
    }

    void physicsWorld::addCollider(boxCollider* collider)
    {
        auto data = new colliderData();
        data->body = _physics->createRigidStatic(physx::PxTransform(physx::PxVec3()));

        auto halfSizes = collider->getHalfSizes();
        data->geometry = new physx::PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z);
        auto material = _physics->createMaterial(0.0f, 0.0f, 0.0f);
        data->shape = data->body->createShape(*data->geometry, *material);
        data->body->userData = collider;

        data->transformChangedToken = collider->getNode()->getTransform()->getChangedEvent()->assign(
            [this, collider, data](transform* sender) -> void
        {
            data->body->setGlobalPose(createPose(collider, sender));
        });

        data->body->setGlobalPose(createPose(collider, collider->getNode()->getTransform()));

        _scene->addActor(*data->body);
        _colliders[collider] = data;

        //if (halfSizes.x > 2.0f)
        //{
        //    auto filterData = physx::PxFilterData();
        //    filterData.word0 = 1u;
        //    data->shape->setQueryFilterData(filterData);
        //}
    }

    void physicsWorld::removeCollider(boxCollider* collider)
    {
        auto data = _colliders[collider];
        collider->getNode()->getTransform()->getChangedEvent()->unassign(data->transformChangedToken);

        _scene->removeActor(*data->body);
        data->body->release();
        safeDelete(data->geometry);
        _colliders.erase(collider);
    }

    void physicsWorld::enableQueryOn(vector<boxCollider*>* colliders)
    {
        for (auto collider : (*colliders))
        {
            auto data = _colliders[collider];
            data->shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        }
    }

    void physicsWorld::disableQueryOn(vector<boxCollider*>* colliders)
    {
        for (auto collider : (*colliders))
        {
            auto data = _colliders[collider];
            data->shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
        }
    }

    void physicsWorld::setGroupOn(vector<boxCollider*>* colliders, uint16_t group)
    {
        auto groupFilterData = physx::PxFilterData();
        groupFilterData.word0 = group;

        for (auto collider : (*colliders))
        {
            auto data = _colliders[collider];
            data->shape->setQueryFilterData(groupFilterData);
        }
    }

    bool physicsWorld::intersects(intersectionCollisionTest test)
    {
        auto geometry = _colliders[test.collider]->geometry;
        auto pose = createPose(test.collider, test.transform);

        auto hit = physx::PxOverlapBuffer();
        auto foundIntersection = false;
        physx::PxQueryFilterData filterData = physx::PxQueryFilterData(physx::PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC);
        filterData.data.word0 = test.group;
        if (_scene->overlap(
            *geometry,
            pose,
            hit,
            filterData))
            foundIntersection = true;

        return foundIntersection;
    }

    bool physicsWorld::intersects(intersectionCollisionMultiTest test)
    {
        auto foundIntersection = false;
        auto collidersCount = test.colliders->size();
        for (size_t i = 0; i < collidersCount && !foundIntersection; i++)
        {
            auto collider = (*test.colliders)[i];
            auto transform = (*test.transforms)[i];

            intersectionCollisionTest singleTest;
            singleTest.collider = collider;
            singleTest.transform = transform;
            singleTest.group = test.group;

            if (intersects(singleTest))
                foundIntersection = true;
        }

        return foundIntersection;
    }

    bool physicsWorld::intersects(intersectionCollisionGroupTest test)
    {
        setGroupOn(test.colliders, 1u);
        setGroupOn(test.againstColliders, 1u);

        intersectionCollisionMultiTest multiTest;
        multiTest.colliders = test.colliders;
        multiTest.transforms = test.transforms;
        multiTest.group = 1u;

        auto result = intersects(multiTest);

        setGroupOn(test.colliders, 0u);
        setGroupOn(test.againstColliders, 0u);

        return result;
    }

    sweepCollisionResult physicsWorld::sweepPenetration(sweepCollisionPairTest test)
    {
        auto result = sweepCollisionResult();

        auto dir = glm::normalize(test.offset);
        auto distance = glm::length(test.offset);

        auto geometrySource = physx::PxBoxGeometry(*static_cast<physx::PxBoxGeometry*>(_colliders[test.colliderSource]->geometry));
        auto poseSource = createPose(test.colliderSource, test.transformSource);

        auto geometryTarget = physx::PxBoxGeometry(*static_cast<physx::PxBoxGeometry*>(_colliders[test.colliderTarget]->geometry));
        auto poseTarget = createPose(test.colliderTarget, test.transformTarget);

        physx::PxSweepHit hit;
        if (physx::PxGeometryQuery::sweep(
            physx::PxVec3(dir.x, dir.y, dir.z),
            distance,
            geometrySource,
            poseSource,
            geometryTarget,
            poseTarget,
            hit,
            physx::PxHitFlag::eDEFAULT | physx::PxHitFlag::eMTD))
        {
            auto collision = sweepCollision();
            collision.collider = test.colliderTarget;
            collision.distance = hit.distance;
            collision.normal = vec3(hit.normal.x, hit.normal.y, hit.normal.z);

            result.collided = true;
            result.collisions.push_back(collision);
        }

        return result;
    }

    sweepCollisionResult physicsWorld::sweep(sweepCollisionTest test)
    {
        auto result = sweepCollisionResult();

        auto geometry = _colliders[test.collider]->geometry;
        auto pose = createPose(test.collider, test.transform);

        physx::PxQueryFilterData filterData = physx::PxQueryFilterData(physx::PxQueryFlag::eSTATIC);
        filterData.data.word0 = test.group;
        physx::PxSweepHit hitBuffer[32];
        auto hit = physx::PxSweepBuffer(hitBuffer, 32);
        if (_scene->sweep(
            *geometry,
            pose,
            physx::PxVec3(test.direction.x, test.direction.y, test.direction.z),
            test.distance,
            hit,
            physx::PxHitFlag::eDEFAULT | physx::PxHitFlag::ePRECISE_SWEEP,
            filterData))
        {
            auto touchesCount = hit.getNbTouches();
            for (size_t i = 0; i < touchesCount; i++)
            {
                auto touch = hit.getTouch(static_cast<physx::PxU32>(i));
                auto collidedCollider = reinterpret_cast<boxCollider*>(touch.actor->userData);
                auto normal = vec3(touch.normal.x, touch.normal.y, touch.normal.z);

                if (touch.distance == 0.0f)
                {
                    sweepCollisionPairTest pairTest;
                    pairTest.colliderSource = test.collider;
                    pairTest.transformSource = test.transform;
                    pairTest.offset = test.direction * DECIMAL_TRUNCATION;
                    pairTest.colliderTarget = collidedCollider;
                    pairTest.transformTarget = collidedCollider->getNode()->getTransform();
                    auto mtdResult = sweepPenetration(pairTest);
                    if (mtdResult.collided)
                    {
                        normal = collidedCollider->getObb().findClosestNormal(mtdResult.collisions[0].normal);
                        auto dot = glm::dot(normal, test.direction);
                        if (mathUtils::isClose(dot, 0.0f) || dot > 0.0f)
                            continue;
                    }
                    else
                        continue;
                }

                result.collisions.push_back(sweepCollision(collidedCollider, touch.distance, normal));
                result.collided = true;
            }
        }

        return result;
    }

    sweepCollisionResult physicsWorld::sweep(sweepCollisionMultiTest test)
    {
        auto allResult = sweepCollisionResult();

        auto collidersCount = test.colliders->size();
        for (size_t i = 0; i < collidersCount; i++)
        {
            auto collider = (*test.colliders)[i];
            transform* transform;
            if (test.transforms && i < test.transforms->size())
                transform = (*test.transforms)[i];
            else
                transform = collider->getNode()->getTransform();

            sweepCollisionTest singleTest;
            singleTest.collider = collider;
            singleTest.transform = transform;
            singleTest.direction = test.direction;
            singleTest.distance = test.distance;
            singleTest.group = test.group;
            auto result = sweep(singleTest);

            if (result.collided)
            {
                allResult.collided = true;

                auto collisionsCount = result.collisions;
                for (auto& collision : result.collisions)
                {
                    if (test.findOnlyClosestPerTarget)
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
                                phi::removeIfContains(allResult.collisions, (*alreadyFoundCollision));
                                allResult.collisions.push_back(collision);
                            }
                        }
                        else
                            allResult.collisions.push_back(collision);
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

    sweepCollisionResult physicsWorld::sweep(sweepCollisionGroupTest test)
    {
        setGroupOn(test.colliders, 1u);
        setGroupOn(test.againstColliders, 1u);

        sweepCollisionMultiTest multiTest;
        multiTest.colliders = test.colliders;
        multiTest.transforms = test.transforms;
        multiTest.direction = test.direction;
        multiTest.distance = test.distance;
        multiTest.group = 1u;

        auto result = sweep(multiTest);

        setGroupOn(test.colliders, 0u);
        setGroupOn(test.againstColliders, 0u);

        return result;
    }
}