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

        _dispatcher->release();
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
        auto position = transformPosition + transformOrientation * (transformSize * collider->getCenter());
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
        data->transform = collider->getNode()->getTransform();
        data->body = _physics->createRigidStatic(PxTransform(PxVec3()));
        auto material = _physics->createMaterial(0.0f, 0.0f, 0.0f);
        data->shape = data->body->createShape(PxBoxGeometry(createBoxGeometry(obb.halfSizes)), *material);
        data->body->userData = collider;

        data->transformChangedToken = data->transform->getChangedEvent()->assign(
            [this, collider](transform* sender) -> void
        {
            updateShape(collider);
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

        updateShape(collider);

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
        data->body->release();
        _colliders.erase(collider);
    }

    void physicsWorld::changeTransform(boxCollider* collider, transform* newTransform)
    {
        auto data = _colliders[collider];
        data->transform->getChangedEvent()->unassign(data->transformChangedToken);

        data->transformChangedToken = newTransform->getChangedEvent()->assign(
            [this, collider](transform* sender) -> void
        {
            updateShape(collider);
        });

        data->transform = newTransform;
        updateShape(collider);
    }

    void physicsWorld::updateShape(boxCollider* collider)
    {
        auto data = _colliders[collider];
        auto obb = collider->getLocalObb();
        obb = obb.transform(data->transform);
        data->shape->setGeometry(PxBoxGeometry(createBoxGeometry(obb.halfSizes)));
        data->body->setGlobalPose(createPose(obb));
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
        setGroupOn(test.collidees, 1u);

        intersectionCollisionMultiTest multiTest;
        multiTest.colliders = test.colliders;
        multiTest.transforms = test.transforms;
        multiTest.group = 1u;

        auto result = intersects(multiTest);

        setGroupOn(test.colliders, 0u);
        setGroupOn(test.collidees, 0u);

        return result;
    }

    bool physicsWorld::intersects(intersectionCollisionPairTest test)
    {
        auto result = sweepCollisionResult();

        auto colliderGeometry = createBoxGeometry(test.collider, test.colliderTransform);
        auto colliderPose = createPose(test.collider, test.colliderTransform);

        auto collideeGeometry = createBoxGeometry(test.collidee, test.collideeTransform);
        auto collideePose = createPose(test.collidee, test.collideeTransform);

        int flags = PxHitFlag::eDEFAULT;

        PxSweepHit hit;
        return PxGeometryQuery::sweep(
            PxVec3(1.0, 0.0, 0.0),
            0.0f,
            colliderGeometry, colliderPose,
            collideeGeometry, collideePose,
            hit,
            static_cast<PxHitFlags>(flags),
            test.inflation);
    }

    sweepCollisionResult physicsWorld::sweep(sweepCollisionPairTest test)
    {
        auto result = sweepCollisionResult();

        auto colliderGeometry = createBoxGeometry(test.collider, test.colliderTransform);
        auto colliderPose = createPose(test.collider, test.colliderTransform);

        auto collideeGeometry = createBoxGeometry(test.collidee, test.collideeTransform);
        auto collideePose = createPose(test.collidee, test.collideeTransform);

        auto direction = toPxVec3(test.direction);

        int flags = PxHitFlag::eDEFAULT;
        if (test.checkPenetration)
            flags |= PxHitFlag::eMTD;

        PxSweepHit hit;
        if (PxGeometryQuery::sweep(
            direction,
            test.distance,
            colliderGeometry, colliderPose,
            collideeGeometry, collideePose,
            hit,
            static_cast<PxHitFlags>(flags),
            test.inflation))
        {
            auto collision = sweepCollision();
            collision.collider = test.collider;
            collision.collidee = test.collidee;
            collision.distance = hit.distance;
            collision.normal = vec3(hit.normal.x, hit.normal.y, hit.normal.z);
            collision.position = vec3(hit.position.x, hit.position.y, hit.position.z);
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
                auto collidee = reinterpret_cast<boxCollider*>(touch.actor->userData);
                auto normal = vec3(touch.normal.x, touch.normal.y, touch.normal.z);
                auto isIntersecting = false;

                // Penetration check for invalid normals:
                // The depenetration normal that results from this test does not always represents the correct colliding normal
                // TODO: find better solution
                if (touch.distance == 0.0f)
                {
                    isIntersecting = true;

                    sweepCollisionPairTest pairTest;
                    pairTest.collider = test.collider;
                    pairTest.colliderTransform = test.transform;
                    pairTest.distance = test.distance;
                    pairTest.direction = test.direction;
                    pairTest.collidee = collidee;
                    pairTest.collideeTransform = collidee->getNode()->getTransform();
                    pairTest.inflation = 0.0f;
                    pairTest.checkPenetration = true;
                    auto penetrationResult = sweep(pairTest);

                    //assert(penetrationResult.collided);
                    if (!penetrationResult.collided)
                        continue;

                    normal = collidee->getObb().findClosestNormalTo(penetrationResult.collisions[0].normal);
                    if (test.disregardDivergentNormals)
                    {
                        auto dot = glm::dot(normal, test.direction);
                        if (mathUtils::isClose(dot, 0.0f) || dot > 0.0f)
                            continue;
                    }
                }

                sweepCollision collision;
                collision.collidee = collidee;
                collision.collider = test.collider;
                collision.distance = glm::max(touch.distance - DECIMAL_TRUNCATION, 0.0f);
                collision.normal = normal;
                collision.isIntersecting = isIntersecting;
                collision.position = vec3(touch.position.x, touch.position.y, touch.position.z);
                result.collisions.push_back(collision);
                result.collided = true;
            }
        }

        safeDeleteArray(hitBuffer);

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

                for (auto& collision : result.collisions)
                {
                    if (test.findOnlyClosestPerTarget)
                    {
                        auto alreadyFoundCollision = std::find_if(allResult.collisions.begin(), allResult.collisions.end(),
                            [&collision](sweepCollision const& sc)
                        {
                            return collision.collidee == sc.collidee;
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
        setGroupOn(test.collidees, 1u);

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
        setGroupOn(test.collidees, 0u);

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

            return rayCastResult({ hit });
        }

        return rayCastResult();
    }
}