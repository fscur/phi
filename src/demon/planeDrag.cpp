#include <precompiled.h>

#include "planeDrag.h"

using namespace phi;

namespace demon
{
    vector<transform*>* planeDrag::createOffsetTransforms(vec3 offset)
    {
        auto transformsCount = _currentTransforms.size();
        auto offsetedTransforms = new vector<transform*>(transformsCount);

        for (size_t i = 0; i < transformsCount; ++i)
        {
            auto transform = _currentTransforms.at(i);
            auto position = transform->getPosition() + offset;
            auto rotation = transform->getOrientation();
            auto size = transform->getSize();
            auto offsetTransform = new phi::transform();
            offsetTransform->setLocalPosition(position);
            offsetTransform->setLocalOrientation(rotation);
            offsetTransform->setLocalSize(size);
            offsetedTransforms->at(i) = offsetTransform;
        }

        return offsetedTransforms;
    }

    bool planeDrag::objectFitsInOffsetedPosition(vec3 offset)
    {
        auto offsetedTransforms = createOffsetTransforms(offset);
        intersectionCollisionMultiTest intersectionTest;
        intersectionTest.colliders = &_currentColliders;
        intersectionTest.transforms = offsetedTransforms;

        auto intersectionResult = _physicsWorld->intersects(intersectionTest);

        for (auto transform : (*offsetedTransforms))
            safeDelete(transform);
        safeDelete(offsetedTransforms);

        return intersectionResult;
    }

    sweepCollisionResult* planeDrag::performCollisionSweep(std::vector<transform*>* transforms, vec3 offset)
    {
        sweepCollisionMultiTest sweepTest;
        sweepTest.colliders = &_currentColliders;
        sweepTest.transforms = transforms;
        sweepTest.direction = glm::normalize(offset);
        sweepTest.distance = glm::length(offset);
        sweepTest.findOnlyClosestPerTarget = true;

        return new sweepCollisionResult(_physicsWorld->sweep(sweepTest));
    }

    vector<boxCollider*>* planeDrag::getSweepCollisionResultColliders(sweepCollisionResult* sweepResult)
    {
        auto colliders = new vector<boxCollider*>();

        for (auto collision : sweepResult->collisions)
            colliders->push_back(collision.collider);

        return colliders;
    }

    sweepCollision planeDrag::findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset)
    {
        auto farthestValidCollision = *sweepResult->collisions.begin();

        auto collisionColliders = getSweepCollisionResultColliders(sweepResult);
        auto offsetNormal = glm::normalize(offset);
        auto reverseIterator = sweepResult->collisions.rbegin();
        while (reverseIterator != sweepResult->collisions.rend())
        {
            auto currentCollision = *reverseIterator;
            phi::removeIfContains(*collisionColliders, currentCollision.collider);
            auto offsetedTransforms = createOffsetTransforms(offsetNormal * currentCollision.distance);
            intersectionCollisionGroupTest intersectionTest;
            intersectionTest.colliders = &_currentColliders;
            intersectionTest.transforms = offsetedTransforms;
            intersectionTest.againstColliders = collisionColliders;
            if (!_physicsWorld->intersects(intersectionTest))
            {
                farthestValidCollision = currentCollision;
                reverseIterator = sweepResult->collisions.rend();
            }
            else
                ++reverseIterator;

            for (auto transform : (*offsetedTransforms))
                safeDelete(transform);
            safeDelete(offsetedTransforms);
        }

        safeDelete(collisionColliders);
        return farthestValidCollision;
    }

    vec3 planeDrag::getAdjustedOffset(sweepCollision collision, vec3 offset)
    {
        auto offsetNormal = glm::normalize(offset);

        auto dot = glm::abs(glm::dot(collision.normal, offsetNormal));
        if (mathUtils::isClose(dot, 1.0f))
            return vec3();

        auto normalOffsetCross = glm::normalize(glm::cross(collision.normal, offsetNormal));
        auto adjustedNormal = glm::normalize(glm::cross(collision.normal, normalOffsetCross));
        auto amountOnPlaneNormal = glm::dot(adjustedNormal, _currentPlane.getNormal());
        adjustedNormal -= amountOnPlaneNormal * _currentPlane.getNormal();

        auto adjustedMagnitude = glm::dot(offset, adjustedNormal);

        return adjustedNormal * adjustedMagnitude;
    }

    void planeDrag::moveObject(vec3 offset)
    {
        auto finalOffset = offset;
        if (objectFitsInOffsetedPosition(offset))
        {
            auto sweepResult = performCollisionSweep(&_currentTransforms, offset);
            if (sweepResult->collisions.size() > 0u)
            {
                auto farthestCollision = findFarthestValidCollision(sweepResult, offset);

                auto limitedOffset = glm::normalize(offset) * farthestCollision.distance;
                auto adjustedOffset = getAdjustedOffset(farthestCollision, offset - limitedOffset);

                if (adjustedOffset == vec3())
                    finalOffset = limitedOffset;
                else
                {
                    auto limitedOffsetTransforms = createOffsetTransforms(limitedOffset);
                    auto adjustSweepResult = performCollisionSweep(limitedOffsetTransforms, adjustedOffset);

                    if (adjustSweepResult->collided && adjustSweepResult->collisions.size() > 0u)
                        finalOffset = limitedOffset + glm::normalize(adjustedOffset) * (*adjustSweepResult->collisions.begin()).distance;
                    else
                        finalOffset = limitedOffset + adjustedOffset;

                    safeDelete(adjustSweepResult);
                    for (auto& transform : (*limitedOffsetTransforms))
                        safeDelete(transform);
                    safeDelete(limitedOffsetTransforms);
                }
            }

            safeDelete(sweepResult);
        }

        _currentObject->getTransform()->translate(finalOffset);
    }

    void planeDrag::startDrag(phi::node* object, plane plane)
    {
        _currentObject = object;
        _currentPlane = plane;

        _currentColliders = vector<boxCollider*>();
        _currentTransforms = vector<transform*>();
        _currentObject->traverse<boxCollider>([this](boxCollider* b)
        {
            _currentColliders.push_back(b);
            _currentTransforms.push_back(b->getNode()->getTransform());
        });

        _currentInitialPlanePosition = plane.getOrigin();
        _currentInitialObjectPosition = _currentObject->getTransform()->getLocalPosition();

        _physicsWorld->disableQueryOn(&_currentColliders);
    }

    void planeDrag::updateDrag(ray ray)
    {
        if (!_currentObject)
            return;

        float t;
        ray.intersects(_currentPlane, t);
        auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;

        auto offsetOnPlane = rayCastOnPlanePosition - _currentInitialPlanePosition;
        auto finalPosition = _currentInitialObjectPosition + offsetOnPlane;

        moveObject(finalPosition - _currentObject->getTransform()->getLocalPosition());
    }

    void planeDrag::endDrag()
    {
        _physicsWorld->enableQueryOn(&_currentColliders);
    }
}