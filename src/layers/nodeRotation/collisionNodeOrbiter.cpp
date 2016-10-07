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

    vector<transform*>* collisionNodeOrbiter::createRotatedTransforms(node* node, quat rotation)
    {
        auto transformsCount = _nodesTransforms[node].size();
        auto offsetedTransforms = new vector<transform*>(transformsCount);

        for (size_t i = 0; i < transformsCount; ++i)
        {
            auto transform = _nodesTransforms[node].at(i);
            auto position = transform->getPosition();
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

    float collisionNodeOrbiter::orbit(float angle, plane plane)
    {
        auto currentAngle = angle;
        auto currentRotation = glm::angleAxis(currentAngle, plane.normal);

        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;
            auto transform = node->getTransform();

            auto position = transform->getLocalPosition();
            auto projectedPosition = plane.projectPoint(position);
            auto originToProjectedPosition = projectedPosition - plane.origin;

            auto rotatedOriginToProjectedPosition = currentRotation * originToProjectedPosition;
            auto offset = rotatedOriginToProjectedPosition - originToProjectedPosition;

            if (offset == vec3())
                break;

            auto rotatedTransforms = createRotatedTransforms(node, currentRotation);
            auto direction = glm::normalize(offset);
            auto sweepTest = sweepCollisionMultiTest();
            sweepTest.colliders = &pair.second;
            sweepTest.transforms = rotatedTransforms;
            sweepTest.distance = glm::length(offset);
            sweepTest.direction = direction;
            auto result = _physicsWorld->sweep(sweepTest);

            for (auto transform : (*rotatedTransforms))
                safeDelete(transform);
            safeDelete(rotatedTransforms);

            assert(!isnan(direction.x));

            if (result.collided)
            {
                auto collidedOffset = direction * result.collisions[0].distance;
                rotatedOriginToProjectedPosition = originToProjectedPosition + collidedOffset;

                auto collidedAngle = glm::orientedAngle(glm::normalize(originToProjectedPosition), glm::normalize(rotatedOriginToProjectedPosition), plane.normal);
                if (glm::abs(collidedAngle) < glm::abs(currentAngle))
                {
                    currentAngle = collidedAngle;
                    currentRotation = glm::angleAxis(currentAngle, plane.normal);
                }
            }

            if (currentAngle == 0.0f)
                break;
        }

        for (auto& pair : _nodesColliders)
        {
            auto node = pair.first;
            auto transform = node->getTransform();

            auto orientation = transform->getLocalOrientation();
            auto targetOrientation = currentRotation * orientation;

            auto position = transform->getLocalPosition();
            auto projectedPosition = plane.projectPoint(position);
            auto originToProjectedPosition = projectedPosition - plane.origin;
            auto rotatedOriginToProjectedPosition = currentRotation * originToProjectedPosition;
            auto offset = rotatedOriginToProjectedPosition - originToProjectedPosition;

            transform->translate(offset);
            transform->setLocalOrientation(targetOrientation);
        }

        return currentAngle;
    }
}