#include <precompiled.h>
#include "collisionNodeTranslator.h"

namespace phi
{
    collisionNodeTranslator::collisionNodeTranslator(physicsWorld* physicsWorld) :
        _physicsWorld(physicsWorld),
        _plane(plane()),
        _colliders(vector<boxCollider*>()),
        _transforms(vector<transform*>()),
        _lastTranslationTouchingCollisions(new vector<sweepCollision>()),
        _resolveCollisions(true)
    {
    }

    collisionNodeTranslator::~collisionNodeTranslator()
    {
        safeDelete(_lastTranslationTouchingCollisions);
    }

    void collisionNodeTranslator::addTouchingCollisions(sweepCollisionResult* sweepResult, sweepCollision compareCollision)
    {
        for (auto& collision : sweepResult->collisions)
        {
            auto isSameDistance = mathUtils::isClose(collision.distance, compareCollision.distance);
            if (isSameDistance)
                _lastTranslationTouchingCollisions->push_back(collision);
            else if (collision.distance > compareCollision.distance)
                break;
        }
    }

    vector<transform*>* collisionNodeTranslator::createOffsetedTransforms(vec3 offset)
    {
        auto transformsCount = _transforms.size();
        auto offsetedTransforms = new vector<transform*>(transformsCount);

        for (size_t i = 0; i < transformsCount; ++i)
        {
            auto transform = _transforms.at(i);
            auto position = _nodesDestinationPositions[_nodes.at(i)] + offset;
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
        auto offsetedTransforms = createOffsetedTransforms(offset);
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
        sweepTest.disregardDivergentNormals = false;

        return new sweepCollisionResult(_physicsWorld->sweep(sweepTest));
    }

    vector<boxCollider*>* collisionNodeTranslator::getSweepCollisionResultCollidees(sweepCollisionResult* sweepResult)
    {
        auto collidees = new vector<boxCollider*>();

        for (auto collision : sweepResult->collisions)
            collidees->push_back(collision.collidee);

        return collidees;
    }

    bool collisionNodeTranslator::findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset, sweepCollision& farthestValidCollision)
    {
        farthestValidCollision = *sweepResult->collisions.begin();

        if (farthestValidCollision.isIntersecting)
            return false;

        auto collisionColliders = getSweepCollisionResultCollidees(sweepResult);
        auto offsetNormal = glm::normalize(offset);
        auto reverseIterator = sweepResult->collisions.rbegin();
        while (reverseIterator != sweepResult->collisions.rend())
        {
            auto currentCollision = *reverseIterator;
            phi::removeIfContains(*collisionColliders, currentCollision.collidee);
            auto offsetedTransforms = createOffsetedTransforms(offsetNormal * currentCollision.distance);

            intersectionCollisionGroupTest intersectionTest;
            intersectionTest.colliders = &_colliders;
            intersectionTest.transforms = offsetedTransforms;
            intersectionTest.collidees = collisionColliders;
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
        return true;
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

    vec3 collisionNodeTranslator::resolveCollisions(vec3 offset)
    {
        assert(!isnan(offset.x));
        assert(!isnan(offset.y));
        assert(!isnan(offset.z));
        assert(offset != vec3());

        if (objectFitsInOffsetedPosition(offset))
            return offset;

        auto resolvedOffset = vec3();

        auto destinationTransforms = createOffsetedTransforms(vec3());

        auto sweepResult = performCollisionSweep(destinationTransforms, offset);
        if (sweepResult->collisions.size() > 0u)
        {
            sweepCollision farthestCollision;
            auto foundFarthestCollision = findFarthestValidCollision(sweepResult, offset, farthestCollision);
            if (!foundFarthestCollision)
            {
                resolvedOffset = vec3();
                addTouchingCollisions(sweepResult, sweepResult->collisions[0]);
            }
            else
            {
                auto limitedOffset = glm::normalize(offset) * farthestCollision.distance;
                auto adjustedOffset = getAdjustedOffset(farthestCollision, offset - limitedOffset);

                if (adjustedOffset == vec3())
                {
                    resolvedOffset = limitedOffset;
                    addTouchingCollisions(sweepResult, farthestCollision);
                }
                else
                {
                    auto limitedOffsetTransforms = createOffsetedTransforms(limitedOffset);
                    auto adjustSweepResult = performCollisionSweep(limitedOffsetTransforms, adjustedOffset, 5u);

                    if (adjustSweepResult->collided && adjustSweepResult->collisions.size() > 0u)
                    {
                        auto firstCollision = adjustSweepResult->collisions.begin();
                        resolvedOffset = limitedOffset + glm::normalize(adjustedOffset) * firstCollision->distance;
                        addTouchingCollisions(adjustSweepResult, *firstCollision);
                    }
                    else
                    {
                        resolvedOffset = limitedOffset + adjustedOffset;
                        addTouchingCollisions(sweepResult, farthestCollision);
                    }

                    safeDelete(adjustSweepResult);
                    for (auto& transform : (*limitedOffsetTransforms))
                        safeDelete(transform);
                    safeDelete(limitedOffsetTransforms);
                }
            }
        }

        for (auto& transform : (*destinationTransforms))
            safeDelete(transform);
        safeDelete(destinationTransforms);

        safeDelete(sweepResult);
        return resolvedOffset;
    }

    void collisionNodeTranslator::addRange(const vector<node*>& nodes)
    {
        for (auto& node : nodes)
            addNode(node);
    }

    void collisionNodeTranslator::addNode(node* node)
    {
        _nodes.push_back(node);
        node->traverse<boxCollider>([&](boxCollider* b)
        {
            if (b->getIsEnabled())
            {
                b->disable();
                _colliders.push_back(b);
                _transforms.push_back(b->getNode()->getTransform());
            }
        });

        auto animator = new phi::animator();
        node->addComponent(animator);
        auto translateAnimation = new phi::translateAnimation([node](vec3 position)
        {
            node->getTransform()->setLocalPosition(position);
        }, easingFunctions::easeOutCubic);
        animator->addAnimation(translateAnimation);
        _nodesTranslateAnimations[node] = translateAnimation;
        _nodesDestinationPositions[node] = node->getTransform()->getLocalPosition();
    }

    void collisionNodeTranslator::clear()
    {
        for (auto& collider : _colliders)
            collider->enable();

        for (auto& pair : _nodesTranslateAnimations)
        {
            auto node = pair.first;
            auto animator = node->getComponent<phi::animator>();
            node->removeComponent(animator);
            safeDelete(animator);
            safeDelete(pair.second);
        }

        _nodesTranslateAnimations.clear();

        _colliders.clear();
        _transforms.clear();
        _nodes.clear();
    }

    vec3 collisionNodeTranslator::translate(vec3 offset)
    {
        _lastTranslationTouchingCollisions->clear();

        if (_resolveCollisions)
            offset = resolveCollisions(offset);

        assert(!isnan(offset.x));
        assert(!isnan(offset.y));
        assert(!isnan(offset.z));

        for (auto& node : _nodes)
        {
            auto destination = _nodesDestinationPositions[node];
            destination += offset;
            auto translateAnimation = _nodesTranslateAnimations[node];
            translateAnimation->start(node->getTransform()->getLocalPosition(), destination, 0.2f);
            _nodesDestinationPositions[node] = destination;

            //node->getTransform()->translate(offset);
        }

        return offset;
    }
}