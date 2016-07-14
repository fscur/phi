#include <precompiled.h>

#include "mouseDrag.h"

using namespace phi;

namespace demon
{
    vector<transform*>* mouseDrag::createOffsetTransforms(vec3 offset)
    {
        auto transformsCount = _transforms.size();
        auto offsetedTransforms = new vector<transform*>(transformsCount);

        for (size_t i = 0; i < transformsCount; ++i)
        {
            auto transform = _transforms.at(i);
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

    bool mouseDrag::objectFitsInOffsetedPosition(vec3 offset)
    {
        auto offsetedTransforms = createOffsetTransforms(offset);
        intersectionCollisionMultiTest intersectionTest;
        intersectionTest.colliders = &_colliders;
        intersectionTest.transforms = offsetedTransforms;

        auto intersectionResult = _physicsWorld->intersects(intersectionTest);

        for (auto transform : (*offsetedTransforms))
            safeDelete(transform);
        safeDelete(offsetedTransforms);

        return intersectionResult;
    }

    sweepCollisionResult* mouseDrag::performCollisionSweep(std::vector<transform*>* transforms, vec3 offset)
    {
        sweepCollisionMultiTest sweepTest;
        sweepTest.colliders = &_colliders;
        sweepTest.transforms = transforms;
        sweepTest.direction = glm::normalize(offset);
        sweepTest.distance = glm::length(offset);
        sweepTest.findOnlyClosestPerTarget = true;

        return new sweepCollisionResult(_physicsWorld->sweep(sweepTest));
    }

    vector<boxCollider*>* mouseDrag::getSweepCollisionResultColliders(sweepCollisionResult* sweepResult)
    {
        auto colliders = new vector<boxCollider*>();

        for (auto collision : sweepResult->collisions)
            colliders->push_back(collision.collider);

        return colliders;
    }

    sweepCollision mouseDrag::findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset)
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
            intersectionTest.colliders = &_colliders;
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

    vec3 mouseDrag::getAdjustedOffset(sweepCollision collision, vec3 offset)
    {
        auto offsetNormal = glm::normalize(offset);

        auto dot = glm::abs(glm::dot(collision.normal, offsetNormal));
        if (mathUtils::isClose(dot, 1.0f))
            return vec3();

        auto normalOffsetCross = glm::normalize(glm::cross(collision.normal, offsetNormal));
        auto adjustedNormal = glm::normalize(glm::cross(collision.normal, normalOffsetCross));
        auto amountOnPlaneNormal = glm::dot(adjustedNormal, _plane.getNormal());
        adjustedNormal -= amountOnPlaneNormal * _plane.getNormal();

        auto adjustedMagnitude = glm::dot(offset, adjustedNormal);

        return adjustedNormal * adjustedMagnitude;
    }

    void mouseDrag::moveObject(vec3 offset)
    {
        if (isnan(offset.x) || offset == vec3())
        {
            phi::debug("errou!");
        }

        auto finalOffset = offset;
        if (objectFitsInOffsetedPosition(offset))
        {
            auto sweepResult = performCollisionSweep(&_transforms, offset);
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
                //finalOffset = limitedOffset;
            }

            safeDelete(sweepResult);
        }

        _object->getTransform()->translate(finalOffset);
    }

    vec3 mouseDrag::castRayToPlane(vec2 screenPosition)
    {
        auto ray = _scene->getCamera()->screenPointToRay(screenPosition.x, screenPosition.y);
        float t;
        ray.intersects(_plane, t);
        return ray.getOrigin() + ray.getDirection() * t;
    }

    void mouseDrag::startDrag(int mouseX, int mouseY)
    {
        _physicsWorld->disableQueryOn(&_colliders);
    }

    void mouseDrag::endDrag()
    {
        _physicsWorld->enableQueryOn(&_colliders);
    }
}