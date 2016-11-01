#include <precompiled.h>
#include "collisionNodeRotator.h"

namespace phi
{
    collisionNodeRotator::collisionNodeRotator(physicsWorld* physicsWorld) :
        _physicsWorld(physicsWorld),
        _isSelfCollisionEnabled(false),
        _resolveCollisions(true)
    {
    }

    collisionNodeRotator::~collisionNodeRotator()
    {
    }

    void collisionNodeRotator::addNode(node* node)
    {
        node->traverse<boxCollider>([&](boxCollider* b)
        {
            if (b->getIsEnabled())
            {
                if (!_isSelfCollisionEnabled)
                    b->disable();

                _nodesColliders[node].push_back(b);
                _nodesTransforms[node].push_back(b->getNode()->getTransform());
            }
        });
    }

    void collisionNodeRotator::addRange(const vector<node*>& nodes)
    {
        for (auto& node : nodes)
            addNode(node);
    }

    void collisionNodeRotator::clear()
    {
        for (auto& pair : _nodesColliders)
            for (auto& collider : pair.second)
                collider->enable();

        _nodesColliders.clear();
        _nodesTransforms.clear();
    }

    float collisionNodeRotator::orbit(float angle, plane plane)
    {
        if (!_resolveCollisions)
        {
            orbitNodes(angle, plane);
            return angle;
        }

        auto maximumAngle = findMaximumOrbitPossible(angle, plane);
        if (glm::abs(maximumAngle) > 0.0f)
            orbitNodes(maximumAngle, plane);

        return maximumAngle;
    }

    float collisionNodeRotator::findMaximumOrbitPossible(float angle, plane plane)
    {
        if (isValidOrbit(angle, plane))
            return angle;

        auto angleStep = glm::sign(angle) * PI * 1e-3f; // TODO: find a decimal truncation for angles?
        auto maximumRotation = 0.0f;
        auto testAngle = 0.0f;

        while (glm::abs(testAngle) < glm::abs(angle))
        {
            testAngle += angleStep;
            if (!isValidOrbit(testAngle, plane))
                break;

            maximumRotation = testAngle;
        }

        return maximumRotation;
    }

    bool collisionNodeRotator::isValidOrbit(float angle, plane plane)
    {
        auto rotation = glm::angleAxis(angle, plane.normal);
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;

            auto position = node->getTransform()->getLocalPosition();
            auto projectedPosition = plane.projectPoint(position);
            auto originToProjectedPosition = projectedPosition - plane.origin;

            auto rotatedOriginToProjectedPosition = rotation * originToProjectedPosition;
            auto offset = rotatedOriginToProjectedPosition - originToProjectedPosition;

            auto transforms = createTransformedTransforms(node, offset, rotation);
            auto test = intersection::groupToSceneTest();
            test.colliders = &_nodesColliders[node];
            test.transforms = transforms;

            if (_physicsWorld->intersects(test))
                return false;
        }

        return true;
    }

    void collisionNodeRotator::orbitNodes(float angle, plane plane)
    {
        auto rotation = glm::angleAxis(angle, plane.normal);
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;
            auto transform = node->getTransform();

            auto orientation = transform->getLocalOrientation();
            auto rotatedOrientation = rotation * orientation;

            auto position = transform->getLocalPosition();
            auto projectedPosition = plane.projectPoint(position);
            auto originToProjectedPosition = projectedPosition - plane.origin;
            auto rotatedOriginToProjectedPosition = rotation * originToProjectedPosition;
            auto offset = rotatedOriginToProjectedPosition - originToProjectedPosition;

            transform->translate(offset);
            transform->setLocalOrientation(rotatedOrientation);
        }
    }

    float collisionNodeRotator::rotate(float angle, plane plane)
    {
        if (!_resolveCollisions)
        {
            rotateNodes(angle, plane);
            return angle;
        }

        auto maximumAngle = findMaximumRotationPossible(angle, plane);
        if (glm::abs(maximumAngle) > 0.0f)
            rotateNodes(maximumAngle, plane);

        return maximumAngle;
    }

    float collisionNodeRotator::findMaximumRotationPossible(float angle, plane plane)
    {
        if (isValidRotation(angle, plane))
            return angle;

        auto angleStep = glm::sign(angle) * PI * 1e-3f; // TODO: find a decimal truncation for angles?
        auto maximumRotation = 0.0f;
        auto testAngle = 0.0f;

        while (glm::abs(testAngle) < glm::abs(angle))
        {
            testAngle += angleStep;
            if (!isValidRotation(testAngle, plane))
                break;

            maximumRotation = testAngle;
        }

        return maximumRotation;
    }

    bool collisionNodeRotator::isValidRotation(float angle, plane plane)
    {
        auto rotation = glm::angleAxis(angle, plane.normal);

        auto transforms = assignRotatedTransformsToPhysicsWorld(rotation);
        auto result = testIntersections(transforms);
        assignOriginalTransformsToPhysicsWorld(transforms);

        return result;
    }

    bool collisionNodeRotator::testIntersections(nodeTransforms transforms)
    {
        for (auto& pair : _nodesColliders)
        {
            auto test = intersection::groupToSceneTest();
            test.colliders = &pair.second;
            test.transforms = &transforms[pair.first];

            for (auto boxCollider : pair.second)
                boxCollider->disable();

            auto intersected = _physicsWorld->intersects(test);

            for (auto boxCollider : pair.second)
                boxCollider->enable();

            if (intersected)
                return false;
        }

        return true;
    }

    nodeTransforms collisionNodeRotator::assignRotatedTransformsToPhysicsWorld(quat rotation)
    {
        auto transforms = nodeTransforms();
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;
            transforms[node] = *createTransformedTransforms(node, vec3(), rotation);

            for (size_t i = 0; i < pair.second.size(); ++i)
            {
                auto collider = pair.second[i];
                auto transform = transforms[node][i];
                _physicsWorld->changeTransform(collider, transform);
            }
        }

        return transforms;
    }

    void collisionNodeRotator::assignOriginalTransformsToPhysicsWorld(nodeTransforms transforms)
    {
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;

            for (size_t i = 0; i < pair.second.size(); ++i)
            {
                auto collider = pair.second[i];
                auto originalTransform = collider->getNode()->getTransform();
                _physicsWorld->changeTransform(collider, originalTransform);
                safeDelete(transforms[pair.first][i]);
            }
        }
    }

    vector<transform*>* collisionNodeRotator::createTransformedTransforms(node* node, vec3 offset, quat rotation)
    {
        auto transformsCount = _nodesTransforms[node].size();
        auto offsetedTransforms = new vector<transform*>(transformsCount);

        for (size_t i = 0; i < transformsCount; ++i)
        {
            auto transform = _nodesTransforms[node].at(i);
            auto position = transform->getPosition() + offset;
            auto orientation = rotation * transform->getOrientation();
            auto size = transform->getSize();
            auto offsetTransform = new phi::transform();
            offsetTransform->setLocalPosition(position);
            offsetTransform->setLocalOrientation(orientation);
            offsetTransform->setLocalSize(size);
            offsetedTransforms->at(i) = offsetTransform;
        }

        return offsetedTransforms;
    }

    void collisionNodeRotator::rotateNodes(float angle, plane plane)
    {
        auto rotation = glm::angleAxis(angle, plane.normal);
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;
            auto transform = node->getTransform();

            auto orientation = transform->getLocalOrientation();
            auto rotatedOrientation = rotation * orientation;

            transform->setLocalOrientation(rotatedOrientation);
        }
    }
}