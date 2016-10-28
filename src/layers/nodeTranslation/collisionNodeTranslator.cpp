#include <precompiled.h>
#include <core/string.h>
#include "collisionNodeTranslator.h"

namespace phi
{
    string a;

    collisionNodeTranslator::collisionNodeTranslator(physicsWorld* physicsWorld) :
        _physicsWorld(physicsWorld),
        _plane(plane()),
        _piledUpNodes(vector<node*>()),
        _piledUpTransforms(vector<transform*>()),
        _piledUpColliders(vector<boxCollider*>()),
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

    transform* collisionNodeTranslator::createOffsetedTransform(node* node, vec3 offset)
    {
        auto transform = node->getTransform();
        auto position = _nodesDestinationPositions[node] + offset;
        auto rotation = transform->getOrientation();
        auto size = transform->getSize();
        auto offsetTransform = new phi::transform();
        offsetTransform->setLocalPosition(position);
        offsetTransform->setLocalOrientation(rotation);
        offsetTransform->setLocalSize(size);
        return offsetTransform;
    }

    vector<transform*> collisionNodeTranslator::createOffsetedTransforms(vec3 offset)
    {
        size_t transformsCount = _piledUpTransforms.size();

        auto offsetedTransforms = vector<transform*>(transformsCount);

        for (size_t i = 0; i < transformsCount; ++i)
        {
            auto node = _piledUpNodes.at(i);
            offsetedTransforms.at(i) = createOffsetedTransform(node, offset);
        }

        return offsetedTransforms;
    }

    bool collisionNodeTranslator::objectFitsInOffsetedPosition(vec3 offset)
    {
        auto offsetedTransforms = createOffsetedTransforms(offset);

        intersectionCollisionMultiTest intersectionTest;
        intersectionTest.colliders = &_piledUpColliders;
        intersectionTest.transforms = &offsetedTransforms;

        auto position = offsetedTransforms.at(0)->getPosition();
        a += to_string(position);

        auto intersectionResult = _physicsWorld->intersects(intersectionTest);

        for (auto transform : offsetedTransforms)
            safeDelete(transform);

        return !intersectionResult;
    }

    sweepCollisionResult* collisionNodeTranslator::performCollisionSweep(vector<transform*>& transforms, vec3 offset)
    {
        sweepCollisionMultiTest sweepTest;
        sweepTest.colliders = &_piledUpColliders;
        sweepTest.transforms = &transforms;
        sweepTest.direction = glm::normalize(offset);
        sweepTest.distance = glm::length(offset) + DECIMAL_TRUNCATION;
        sweepTest.findOnlyClosestPerTarget = true;
        sweepTest.disregardDivergentNormals = false;

        return new sweepCollisionResult(_physicsWorld->sweep(sweepTest));
    }

    vector<boxCollider*> collisionNodeTranslator::getSweepCollisionResultCollidees(sweepCollisionResult* sweepResult)
    {
        auto collidees = vector<boxCollider*>();

        for (auto collision : sweepResult->collisions)
            collidees.push_back(collision.collidee);

        return collidees;
    }

    bool collisionNodeTranslator::findFarthestValidCollision(sweepCollisionResult* sweepResult, vec3 offset, sweepCollision& farthestValidCollision)
    {
        farthestValidCollision = *sweepResult->collisions.begin();

        //if (farthestValidCollision.isIntersecting)
        //    return false;

        auto collisionColliders = getSweepCollisionResultCollidees(sweepResult);
        auto offsetNormal = glm::normalize(offset);
        auto reverseIterator = sweepResult->collisions.rbegin();
        while (reverseIterator != sweepResult->collisions.rend())
        {
            auto currentCollision = *reverseIterator;
            removeIfContains(collisionColliders, currentCollision.collidee);
            auto offsetedTransforms = createOffsetedTransforms(offsetNormal * currentCollision.distance);

            intersectionCollisionGroupTest intersectionTest;
            intersectionTest.colliders = &_piledUpColliders;
            intersectionTest.transforms = &offsetedTransforms;
            intersectionTest.collidees = &collisionColliders;

            auto intersects = _physicsWorld->intersects(intersectionTest);

            for (auto& transform : offsetedTransforms)
                safeDelete(transform);

            if (!intersects)
            {
                farthestValidCollision = currentCollision;
                break;
            }

            ++reverseIterator;
        }

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
        {
            a += "fits offset";
            return offset;
        }

        a += "does not fits offset -> ";

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

                a += "no farthest collision: vec3() ";
            }
            else
            {
                a += "farthest collision found [d = " + std::to_string(farthestCollision.distance) + "] -> ";

                auto limitedOffset = glm::normalize(offset) * farthestCollision.distance;
                limitedOffset = limitedOffset + farthestCollision.normal * DECIMAL_TRUNCATION;

                auto adjustedOffset = getAdjustedOffset(farthestCollision, offset - limitedOffset);

                if (adjustedOffset == vec3())
                {
                    resolvedOffset = limitedOffset;
                    addTouchingCollisions(sweepResult, farthestCollision);

                    a += "no adjust: farthest limited offset";
                }
                else
                {
                    a += "has adjust offset -> ";

                    auto limitedOffsetTransforms = createOffsetedTransforms(limitedOffset);
                    auto adjustSweepResult = performCollisionSweep(limitedOffsetTransforms, adjustedOffset);

                    if (adjustSweepResult->collided && adjustSweepResult->collisions.size() > 0u)
                    {
                        a += "collided on adjust: farthest limited offset + collided adjusted offset ";
                        auto firstCollision = adjustSweepResult->collisions.begin();
                        resolvedOffset = limitedOffset + glm::normalize(adjustedOffset) * firstCollision->distance;
                        addTouchingCollisions(adjustSweepResult, *firstCollision);
                    }
                    else
                    {
                        a += "no collision on adjust: farthest limited offset + adjusted offset ";
                        resolvedOffset = limitedOffset + adjustedOffset;
                        addTouchingCollisions(sweepResult, farthestCollision);
                    }

                    safeDelete(adjustSweepResult);
                    for (auto& transform : limitedOffsetTransforms)
                        safeDelete(transform);
                }
            }
        }
        else
        {
            a += "no sweep results: vec3()";
        }

        for (auto& transform : destinationTransforms)
            safeDelete(transform);

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
        node->traverseNodesContaining<boxCollider>([&](phi::node* traversedNode, boxCollider* collider)
        {
            collider->disable();
            auto transform = traversedNode->getTransform();
            _piledUpNodes.push_back(traversedNode);
            _piledUpTransforms.push_back(transform);
            _piledUpColliders.push_back(collider);
            _nodesDestinationPositions[traversedNode] = transform->getPosition();
        });

        auto animator = new phi::animator();
        node->addComponent(animator);
        auto translateAnimation = new phi::translateAnimation([node](vec3 position)
        {
            node->getTransform()->setLocalPosition(position);
        }, easingFunctions::easeOutCubic);
        animator->addAnimation(translateAnimation);
        _nodesTranslateAnimations[node] = translateAnimation;
        _nodesDestinationPositions[node] = node->getTransform()->getPosition();
    }

    void collisionNodeTranslator::clear()
    {
        for (auto& collider : _piledUpColliders)
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
        _nodesDestinationPositions.clear();

        _piledUpNodes.clear();
        _piledUpTransforms.clear();
        _piledUpColliders.clear();
        _nodes.clear();
    }

    void collisionNodeTranslator::translateNodes(vec3 offset)
    {
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

            node->traverseNodesContaining<boxCollider>([&](phi::node* traversedNode, boxCollider* collider)
            {
                auto destination = _nodesDestinationPositions[traversedNode];
                destination += offset;
                _nodesDestinationPositions[traversedNode] = destination;
            });

            //auto newTranslatedObb = obb(*node->getObb());
            //newTranslatedObb.center = destination;
            //auto newLeavingPosition = newTranslatedObb.getPositionAt(vec3(0.0f, 0.0f, -1.0f));
            //debug(to_string(newLeavingPosition));

            //debug(to_string(destination) + "| " + a);
            //debug(to_string(destination));
        }
    }

    vec3 collisionNodeTranslator::translate(vec3 offset)
    {
        _lastTranslationTouchingCollisions->clear();
        a = "";
        if (_resolveCollisions)
        {
            auto resolvedOffset = resolveCollisions(offset);
            translateNodes(resolvedOffset);
            return resolvedOffset;
        }

        translateNodes(offset);
        return offset;
    }
}