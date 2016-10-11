#include <precompiled.h>
#include "collisionNodeOrbiter.h"

namespace phi
{
    collisionNodeOrbiter::collisionNodeOrbiter(physicsWorld* physicsWorld) :
        _physicsWorld(physicsWorld)
    {
    }

    collisionNodeOrbiter::~collisionNodeOrbiter()
    {
    }

    vector<transform*>* collisionNodeOrbiter::createTransformedTransforms(node* node, vec3 offset, quat rotation)
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

    void collisionNodeOrbiter::addRange(const vector<node*>& nodes)
    {
        for (auto& node : nodes)
            addNode(node);
    }

    void collisionNodeOrbiter::addNode(node* node)
    {
        node->traverse<boxCollider>([&](boxCollider* b)
        {
            if (b->getIsEnabled())
            {
                b->disable();
                _nodesColliders[node].push_back(b);
                _nodesTransforms[node].push_back(b->getNode()->getTransform());
            }
        });
    }

    void collisionNodeOrbiter::clear()
    {
        for (auto& pair : _nodesColliders)
            for (auto& collider : pair.second)
                collider->enable();

        _nodesColliders.clear();
        _nodesTransforms.clear();
    }

    bool collisionNodeOrbiter::isValidOrbit(float angle, plane plane)
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
            auto test = intersectionCollisionMultiTest();
            test.colliders = &_nodesColliders[node];
            test.transforms = transforms;

            if (_physicsWorld->intersects(test))
                return false;
        }

        return true;
    }

    bool collisionNodeOrbiter::isValidRotation(float angle, plane plane)
    {
        auto rotation = glm::angleAxis(angle, plane.normal);
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;

            auto transforms = createTransformedTransforms(node, vec3(), rotation);
            auto test = intersectionCollisionMultiTest();
            test.colliders = &_nodesColliders[node];
            test.transforms = transforms;

            if (_physicsWorld->intersects(test))
                return false;
        }

        return true;
    }

    float collisionNodeOrbiter::findMaximumOrbitPossible(float angle, plane plane)
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

    float collisionNodeOrbiter::findMaximumRotationPossible(float angle, plane plane)
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

    float collisionNodeOrbiter::orbit(float angle, plane plane)
    {
        auto maximumAngle = findMaximumOrbitPossible(angle, plane);
        if (maximumAngle == 0.0f)
            return 0.0f;

        auto rotation = glm::angleAxis(maximumAngle, plane.normal);
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

        return maximumAngle;
    }

    float collisionNodeOrbiter::rotate(float angle, plane plane)
    {
        auto maximumAngle = findMaximumOrbitPossible(angle, plane);
        if (maximumAngle == 0.0f)
            return 0.0f;

        auto rotation = glm::angleAxis(maximumAngle, plane.normal);
        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;
            auto transform = node->getTransform();

            auto orientation = transform->getLocalOrientation();
            auto rotatedOrientation = rotation * orientation;

            transform->setLocalOrientation(rotatedOrientation);
        }

        return maximumAngle;
    }
}