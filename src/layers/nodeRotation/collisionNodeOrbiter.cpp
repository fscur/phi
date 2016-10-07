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

    float collisionNodeOrbiter::orbit(float angle, plane plane)
    {
        auto angleStep = PI * 1e-3f; // TODO: find a decimal truncation for angles?
        auto currentAngle = angle;
        auto currentRotation = glm::angleAxis(currentAngle, plane.normal);

        auto collidedWithAnything = true;
        while (collidedWithAnything && glm::abs(currentAngle - angle) < glm::abs(angle))
        {
            collidedWithAnything = false;
            for (auto& pair : _nodesColliders)
            {
                auto node = pair.first;

                auto position = node->getTransform()->getLocalPosition();
                auto projectedPosition = plane.projectPoint(position);
                auto originToProjectedPosition = projectedPosition - plane.origin;

                auto rotatedOriginToProjectedPosition = currentRotation * originToProjectedPosition;
                auto offset = rotatedOriginToProjectedPosition - originToProjectedPosition;

                auto transforms = createTransformedTransforms(node, offset, currentRotation);
                auto test = intersectionCollisionMultiTest();
                test.colliders = &_nodesColliders[node];
                test.transforms = transforms;

                if (_physicsWorld->intersects(test))
                {
                    collidedWithAnything = true;
                    break;
                }
            }

            if (collidedWithAnything)
            {
                currentAngle -= glm::sign(angle) * angleStep;
                currentRotation = glm::angleAxis(currentAngle, plane.normal);
            }
        }

        if (currentAngle == 0.0f)
            return currentAngle;

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