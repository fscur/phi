#include <precompiled.h>

#include "collisionNodeTranslator.h"

namespace phi
{
    collisionNodeTranslator::collisionNodeTranslator(physicsWorld* physicsWorld) :
        _physicsWorld(physicsWorld),
        _plane(vec3(), vec3()),
        _node(nullptr),
        _colliders(vector<boxCollider*>()),
        _transforms(vector<transform*>()),
        _lastTranslationTouchingCollisions(nullptr)
    {
    }

    vector<sweepCollision>* collisionNodeTranslator::getTouchingCollisions(sweepCollisionResult* sweepResult, sweepCollision compareCollision)
    {
        auto touchingCollisions = new vector<sweepCollision>();

        for (auto& collision : sweepResult->collisions)
        {
            if (mathUtils::isClose(collision.distance, compareCollision.distance))
                touchingCollisions->push_back(collision);
            else if (collision.distance > compareCollision.distance)
                break;
        }

        return touchingCollisions;
    }

    vector<transform*>* collisionNodeTranslator::createOffsetTransforms(vec3 offset)
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

    bool collisionNodeTranslator::objectFitsInOffsetedPosition(vec3 offset)
    {
        auto offsetedTransforms = createOffsetTransforms(offset);
        intersectionCollisionMultiTest intersectionTest;
        intersectionTest.colliders = &_colliders;
        intersectionTest.transforms = offsetedTransforms;

        auto intersectionResult = _physicsWorld->intersects(intersectionTest);

        for (auto transform : (*offsetedTransforms))
            safeDelete(transform);
        safeDelete(offsetedTransforms);

        return !intersectionResult;
    }

    sweepCollisionResult* collisionNodeTranslator::performCollisionSweep(vector<transform*>* transforms, vec3 offset, uint32_t maximumHits)
    {
        sweepCollisionMultiTest sweepTest;
        sweepTest.colliders = &_colliders;
        sweepTest.transforms = transforms;
        sweepTest.direction = glm::normalize(offset);
        sweepTest.distance = glm::length(offset);
        sweepTest.findOnlyClosestPerTarget = true;
        sweepTest.maximumHits = maximumHits;

        return new sweepCollisionResult(_physicsWorld->sweep(sweepTest));
    }

    vector<boxCollider*>* collisionNodeTranslator::getSweepCollisionResultColliders(sweepCollisionResult* sweepResult)
    {
        auto colliders = new vector<boxCollider*>();

        for (auto collision : sweepResult->collisions)
            colliders->push_back(collision.collider);

        return colliders;
    }

    sweepCollision collisionNodeTranslator::findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset)
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

    vec3 collisionNodeTranslator::getAdjustedOffset(sweepCollision collision, vec3 offset)
    {
        auto offsetNormal = glm::normalize(offset);

        auto dot = glm::abs(glm::dot(collision.normal, offsetNormal));
        if (mathUtils::isClose(dot, 1.0f))
            return vec3();

        auto normalOffsetCross = glm::normalize(glm::cross(collision.normal, offsetNormal));
        auto adjustedNormal = glm::normalize(glm::cross(collision.normal, normalOffsetCross));
        auto amountOnPlaneNormal = glm::dot(adjustedNormal, _plane.normal);
        adjustedNormal -= amountOnPlaneNormal * _plane.normal;

        auto adjustedMagnitude = glm::dot(offset, adjustedNormal);

        return adjustedNormal * adjustedMagnitude;
    }

    vec3 collisionNodeTranslator::getUndisruptedOffset(vec3 offset)
    {
        assert(!isnan(offset.x));
        assert(!isnan(offset.y));
        assert(!isnan(offset.z));
        assert(offset != vec3());

        if (objectFitsInOffsetedPosition(offset))
            return offset;

        auto finalOffset = vec3();

        auto sweepResult = performCollisionSweep(&_transforms, offset);
        if (sweepResult->collisions.size() > 0u)
        {
            auto farthestCollision = findFarthestValidCollision(sweepResult, offset);

            auto limitedOffset = glm::normalize(offset) * farthestCollision.distance;
            auto adjustedOffset = getAdjustedOffset(farthestCollision, offset - limitedOffset);

            if (adjustedOffset == vec3())
            {
                finalOffset = limitedOffset;
                _lastTranslationTouchingCollisions = getTouchingCollisions(sweepResult, farthestCollision);
            }
            else
            {
                auto limitedOffsetTransforms = createOffsetTransforms(limitedOffset);
                auto adjustSweepResult = performCollisionSweep(limitedOffsetTransforms, adjustedOffset, 5u);

                if (adjustSweepResult->collided && adjustSweepResult->collisions.size() > 0u)
                {
                    auto firstCollision = adjustSweepResult->collisions.begin();
                    finalOffset = limitedOffset + glm::normalize(adjustedOffset) * firstCollision->distance;
                    _lastTranslationTouchingCollisions = getTouchingCollisions(adjustSweepResult, *firstCollision);
                }
                else
                {
                    finalOffset = limitedOffset + adjustedOffset;
                    _lastTranslationTouchingCollisions = getTouchingCollisions(sweepResult, farthestCollision);
                }

                safeDelete(adjustSweepResult);
                for (auto& transform : (*limitedOffsetTransforms))
                    safeDelete(transform);
                safeDelete(limitedOffsetTransforms);
            }
        }

        safeDelete(sweepResult);

        return finalOffset;
    }

    void collisionNodeTranslator::setNode(node* node)
    {
        _node = node;
        _colliders = vector<boxCollider*>();
        _transforms = vector<transform*>();
        _node->traverse<boxCollider>([this](boxCollider* b)
        {
            _colliders.push_back(b);
            _transforms.push_back(b->getNode()->getTransform());
        });
    }

    void collisionNodeTranslator::beginTranslations()
    {
        _physicsWorld->disableQueryOn(&_colliders);
    }

    void collisionNodeTranslator::translateNode(vec3 offset)
    {
        if (_lastTranslationTouchingCollisions)
            safeDelete(_lastTranslationTouchingCollisions);

        auto validOffset = getUndisruptedOffset(offset);
        _node->getTransform()->translate(validOffset);
    }

    void collisionNodeTranslator::endTranslations()
    {
        _physicsWorld->enableQueryOn(&_colliders);
    }
}