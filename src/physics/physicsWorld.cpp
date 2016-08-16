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

    PxVec3 physicsWorld::toPxVec3(const vec3 vector)
    {
        return PxVec3(vector.x, vector.y, vector.z);
    }

    PxTransform physicsWorld::createPose(const obb& obb)
    {
        auto position = obb.center;
        auto rotationMatrix = PxMat33(
            toPxVec3(obb.axes[0]),
            toPxVec3(obb.axes[1]),
            toPxVec3(obb.axes[2]));

        return PxTransform(
            toPxVec3(position),
            PxQuat(rotationMatrix));
    }

    PxTransform physicsWorld::createPose(const boxCollider* collider, transform* transform)
    {
        auto transformPosition = transform->getPosition();
        auto transformOrientation = transform->getOrientation();
        auto transformSize = transform->getSize();
        auto position = transformPosition + transformOrientation * transformSize * collider->getCenter();
        auto axisX = transformOrientation * vec3(1.0f, 0.0f, 0.0f);
        auto axisY = transformOrientation * vec3(0.0f, 1.0f, 0.0f);
        auto axisZ = transformOrientation * vec3(0.0f, 0.0f, 1.0f);

        return PxTransform(
            PxVec3(position.x, position.y, position.z),
            PxQuat(
                PxMat33(
                    toPxVec3(axisX),
                    toPxVec3(axisY),
                    toPxVec3(axisZ)))
        );
    }

    PxBoxGeometry physicsWorld::createBoxGeometry(const vec3& halfSizes)
    {
        return PxBoxGeometry(toPxVec3(halfSizes));
    }

    PxBoxGeometry physicsWorld::createBoxGeometry(const boxCollider* collider, transform* transform)
    {
        auto halfSizes = collider->getSize() * transform->getSize() * 0.5f;
        return createBoxGeometry(halfSizes);
    }

    void physicsWorld::addCollider(boxCollider* collider)
    {
        auto obb = collider->getObb();
        auto data = new colliderData();
        data->body = _physics->createRigidStatic(PxTransform(PxVec3()));

        data->geometry = new PxBoxGeometry(createBoxGeometry(obb.halfSizes));
        auto material = _physics->createMaterial(0.0f, 0.0f, 0.0f);
        data->shape = data->body->createShape(*data->geometry, *material);
        data->body->userData = collider;
        data->body->setGlobalPose(createPose(obb));

        data->transformChangedToken = collider->getNode()->getTransform()->getChangedEvent()->assign(
            [this, collider, data](transform* sender) -> void
        {
            auto obb = collider->getObb();
            safeDelete(data->geometry); // I hate this pointer
            data->geometry = new PxBoxGeometry(createBoxGeometry(obb.halfSizes));
            data->shape->setGeometry(*data->geometry);
            data->body->setGlobalPose(createPose(obb));
        });

        data->isEnabledChangedToken = collider->getIsEnabledChanged()->assign(
            [this](boxCollider* collider) -> void
        {
            if (collider->getIsEnabled())
                enableQueryOn(collider);
            else
                disableQueryOn(collider);
        });

        _scene->addActor(*data->body);
        _colliders[collider] = data;

        if (collider->getIsEnabled())
            enableQueryOn(collider);
        else
            disableQueryOn(collider);
    }

    void physicsWorld::removeCollider(boxCollider* collider)
    {
        auto data = _colliders[collider];
        collider->getNode()->getTransform()->getChangedEvent()->unassign(data->transformChangedToken);
        collider->getIsEnabledChanged()->unassign(data->isEnabledChangedToken);

        _scene->removeActor(*data->body);
        safeDelete(data->geometry);
        data->body->release();
        _colliders.erase(collider);
    }

    void physicsWorld::enableQueryOn(boxCollider* collider)
    {
        auto data = _colliders[collider];
        data->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    }

    void physicsWorld::enableQueryOn(vector<boxCollider*>* colliders)
    {
        for (auto collider : (*colliders))
            enableQueryOn(collider);
    }

    void physicsWorld::disableQueryOn(boxCollider* collider)
    {
        auto data = _colliders[collider];
        data->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
    }

    void physicsWorld::disableQueryOn(vector<boxCollider*>* colliders)
    {
        for (auto collider : (*colliders))
            disableQueryOn(collider);
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
        auto geometry = createBoxGeometry(test.collider, test.transform);
        auto pose = createPose(test.collider, test.transform);

        auto hit = PxOverlapBuffer();
        PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC);
        filterData.data.word0 = test.group;
        if (_scene->overlap(
            geometry,
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

    sweepCollisionResult physicsWorld::sweep(sweepCollisionPairTest test)
    {
        auto result = sweepCollisionResult();

        auto geometrySource = createBoxGeometry(test.sourceCollider, test.targetTransform);
        auto poseSource = createPose(test.sourceCollider, test.sourceTransform);

        auto geometryTarget = createBoxGeometry(test.targetCollider, test.targetTransform);
        auto poseTarget = createPose(test.targetCollider, test.targetTransform);

        auto direction = toPxVec3(test.direction);

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

        auto geometry = createBoxGeometry(test.collider, test.transform);
        auto pose = createPose(test.collider, test.transform);

        int flags = PxHitFlag::eDEFAULT;
        if (test.inflation == 0.0f)
            flags |= PxHitFlag::ePRECISE_SWEEP;

        auto filterData = PxQueryFilterData(PxQueryFlag::eSTATIC);
        filterData.data.word0 = test.group;
        auto hitBuffer = new PxSweepHit[test.maximumHits];
        auto hit = PxSweepBuffer(hitBuffer, test.maximumHits);
        if (_scene->sweep(
            geometry,
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

        auto origin = toPxVec3(rayOrigin);
        auto direction = toPxVec3(rayDirection);

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