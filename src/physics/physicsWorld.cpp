#include <precompiled.h>

#include <core\node.h>
#include "physicsWorld.h"

using namespace physx;

namespace phi
{
    physicsWorld::physicsWorld() :
        _colliders(unordered_map<boxCollider*, colliderData*>())
    {
        _foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocatorCallback, _errorCallback);
        if (!_foundation)
            phi::debug("PxCreateFoundation failed!"); // TODO: throw exception

        _profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(_foundation);
        if (!_profileZoneManager)
            phi::debug("PxProfileZoneManager::createProfileZoneManager failed!"); // TODO: throw exception

        _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, PxTolerancesScale(), true, _profileZoneManager);
        if (!_physics)
            phi::debug("PxCreatePhysics failed!"); // TODO: throw exception

        PxSceneDesc sceneDesc(_physics->getTolerancesScale());
        _dispatcher = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = _dispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
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

    PxTransform physicsWorld::createPose(const boxCollider* collider, transform* transform)
    {
        auto rotation = transform->getOrientation();
        auto center = rotation * collider->getPosition();
        auto position = center + transform->getPosition();

        return PxTransform(
            PxVec3(position.x, position.y, position.z),
            PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
    }

    void physicsWorld::addCollider(boxCollider* collider)
    {
        auto data = new colliderData();
        data->body = _physics->createRigidStatic(PxTransform(PxVec3()));

        auto halfSizes = collider->getHalfSizes();
        data->geometry = new PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z);
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
            data->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        }
    }

    void physicsWorld::disableQueryOn(vector<boxCollider*>* colliders)
    {
        for (auto collider : (*colliders))
        {
            auto data = _colliders[collider];
            data->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
        }
    }

    void physicsWorld::setGroupOn(vector<boxCollider*>* colliders, uint16_t group)
    {
        auto groupFilterData = PxFilterData();
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

        auto hit = PxOverlapBuffer();
        PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC);
        filterData.data.word0 = test.group;
        if (_scene->overlap(
            *geometry,
            pose,
            hit,
            filterData))
            return true;

        return false;
    }

    bool physicsWorld::intersects(intersectionCollisionMultiTest test)
    {
        auto collidersCount = test.colliders->size();
        for (size_t i = 0; i < collidersCount; i++)
        {
            auto collider = (*test.colliders)[i];
            transform* transform;
            if (test.transforms && i < test.transforms->size())
                transform = (*test.transforms)[i];
            else
                transform = collider->getNode()->getTransform();

            intersectionCollisionTest singleTest;
            singleTest.collider = collider;
            singleTest.transform = transform;
            singleTest.group = test.group;

            if (intersects(singleTest))
                return true;
        }

        return false;
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

    sweepCollisionResult physicsWorld::touchs(intersectionCollisionMultiTest test)
    {
        sweepCollisionMultiTest sweepTest;
        sweepTest.colliders = test.colliders;
        sweepTest.transforms = test.transforms;
        sweepTest.group = test.group;
        sweepTest.disregardDivergentNormals = false;
        sweepTest.distance = 0.0f;
        sweepTest.direction = vec3(1.0f, 0.0f, 0.0f);
        sweepTest.inflation = DECIMAL_TRUNCATION;

        return sweep(sweepTest);
    }

    sweepCollisionResult physicsWorld::sweep(sweepCollisionPairTest test)
    {
        auto result = sweepCollisionResult();

        auto geometrySource = PxBoxGeometry(*static_cast<PxBoxGeometry*>(_colliders[test.sourceCollider]->geometry));
        auto poseSource = createPose(test.sourceCollider, test.sourceTransform);

        auto geometryTarget = PxBoxGeometry(*static_cast<PxBoxGeometry*>(_colliders[test.targetCollider]->geometry));
        auto poseTarget = createPose(test.targetCollider, test.targetTransform);

        auto direction = PxVec3(test.direction.x, test.direction.y, test.direction.z);

        int flags = PxHitFlag::eDEFAULT;
        if (test.checkPenetration)
            flags |= PxHitFlag::eMTD;

        PxSweepHit hit;
        if (PxGeometryQuery::sweep(
            direction,
            test.distance,
            geometrySource, poseSource,
            geometryTarget, poseTarget,
            hit,
            static_cast<PxHitFlags>(flags),
            test.inflation))
        {
            auto collision = sweepCollision();
            collision.sourceCollider = test.sourceCollider;
            collision.collider = test.targetCollider;
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

        int flags = PxHitFlag::eDEFAULT;
        if (test.inflation == 0.0f)
            flags |= PxHitFlag::ePRECISE_SWEEP;

        auto filterData = PxQueryFilterData(PxQueryFlag::eSTATIC);
        filterData.data.word0 = test.group;
        auto hitBuffer = new PxSweepHit[test.maximumHits];
        auto hit = PxSweepBuffer(hitBuffer, test.maximumHits);
        if (_scene->sweep(
            *geometry,
            pose,
            PxVec3(test.direction.x, test.direction.y, test.direction.z),
            test.distance,
            hit,
            static_cast<PxHitFlags>(flags),
            filterData,
            nullptr,
            nullptr,
            test.inflation))
        {
            auto touchesCount = hit.getNbTouches();
            for (size_t i = 0; i < touchesCount; i++)
            {
                auto touch = hit.getTouch(static_cast<PxU32>(i));
                auto collider = reinterpret_cast<boxCollider*>(touch.actor->userData);
                auto normal = vec3(touch.normal.x, touch.normal.y, touch.normal.z);

                // TODO: check if this is going to be necessary
                // Penetration check for invalid normals
                //if (touch.distance == 0.0f)
                //{
                //    sweepCollisionPairTest pairTest;
                //    pairTest.colliderSource = test.collider;
                //    pairTest.transformSource = test.transform;
                //    pairTest.distance = DECIMAL_TRUNCATION;
                //    pairTest.direction = test.direction;
                //    pairTest.colliderTarget = collidedCollider;
                //    pairTest.transformTarget = collidedCollider->getNode()->getTransform();
                //    pairTest.inflation = test.inflation;
                //    pairTest.checkPenetration = true;
                //    auto penetrationResult = sweep(pairTest);
                //    if (penetrationResult.collided)
                //    {
                //        normal = collidedCollider->getObb().findClosestNormal(penetrationResult.collisions[0].normal);
                //        auto dot = glm::dot(normal, test.direction);
                //        if (test.disregardDivergentNormals && (mathUtils::isClose(dot, 0.0f) || dot > 0.0f))
                //            continue;
                //    }
                //    else
                //        continue;
                //}

                result.collisions.push_back(sweepCollision(test.collider, collider, touch.distance - DECIMAL_TRUNCATION, normal));
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
            singleTest.disregardDivergentNormals = test.disregardDivergentNormals;
            singleTest.inflation = test.inflation;
            singleTest.maximumHits = test.maximumHits;
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
        multiTest.maximumHits = test.maximumHits;
        multiTest.group = 1u;
        multiTest.disregardDivergentNormals = test.disregardDivergentNormals;
        multiTest.inflation = test.inflation;

        auto result = sweep(multiTest);

        setGroupOn(test.colliders, 0u);
        setGroupOn(test.againstColliders, 0u);

        return result;
    }

    rayCastResult physicsWorld::rayCast(const rayCastTest& test)
    {
        PxQueryFilterData queryFilter;
        queryFilter.flags |= PxQueryFlag::eANY_HIT; // note the OR with the default value

        auto rayDirection = test.ray.getDirection();
        auto rayOrigin = test.ray.getOrigin();

        auto origin = PxVec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
        auto direction = PxVec3(rayDirection.x, rayDirection.y, rayDirection.z);
        
        PxRaycastBuffer hit;

        bool status = _scene->raycast(origin, direction, test.maxDistance, hit, PxHitFlags(PxHitFlag::eDEFAULT), queryFilter);

        if (status)
        {
            auto firstHit = hit.getAnyHit(0);
            auto firstHitNormal = firstHit.normal;
            auto firstHitPosition = firstHit.position + firstHitNormal * DECIMAL_TRUNCATION;
            auto firstHitBoxCollider = reinterpret_cast<boxCollider*>(firstHit.actor->userData);

            auto hit = rayCastHit();
            hit.position = vec3(firstHitPosition.x, firstHitPosition.y, firstHitPosition.z);
            hit.normal = vec3(firstHitNormal.x, firstHitNormal.y, firstHitNormal.z);
            hit.collider = firstHitBoxCollider;

            return rayCastResult({hit});
        }

        return rayCastResult();
    }
}