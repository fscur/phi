#include <precompiled.h>

#include "planesTranslationInputController.h"

namespace phi
{
    planesTranslationInputController::planesTranslationInputController(camera* camera, layer* planesLayer, physicsLayerBehaviour* physicsBehaviour) :
        translationInputController(camera, planesLayer),
        _physicsBehaviour(physicsBehaviour)
    {
    }

    bool planesTranslationInputController::canTranslateAt(sweepCollision collision)
    {
        auto planePosition = _draggingCollider->getObb().getPositionAt(-collision.normal);
        auto cameraPosition = _camera->getTransform()->getPosition();

        auto toPlaneDir = glm::normalize(planePosition - cameraPosition);

        auto discardDot = glm::abs(glm::dot(collision.normal, toPlaneDir));
        return discardDot > 0.2f;
    }

    vector<sweepCollision> planesTranslationInputController::findTouchCollisions()
    {
        vector<sweepCollision> touchs;

        intersectionCollisionMultiTest touchTest;
        touchTest.colliders = _collisionNodeTranslator->getColliders();
        touchTest.transforms = _collisionNodeTranslator->getTransforms();
        auto touchTestResult = _physicsBehaviour->getPhysicsWorld()->touchs(touchTest);

        if (!touchTestResult.collided)
            return touchs;

        for (auto& collision : touchTestResult.collisions)
        {
            if (canTranslateAt(collision))
                touchs.push_back(collision);
        }

        return touchs;
    }

    bool planesTranslationInputController::existsTranslationPlaneWithNormal(vec3 normal)
    {
        auto translationPlane = std::find_if(_translationPlanes.begin(), _translationPlanes.end(),
            [normal](phi::translationPlane* tp)
        {
            return mathUtils::isClose(glm::dot(normal, tp->plane.normal), 1.0f);
        });

        return translationPlane != _translationPlanes.end();
    }

    void planesTranslationInputController::addPlanesIfNeeded(vector<sweepCollision> touchs)
    {
        for (auto& collision : touchs)
        {
            auto collisionNormal = collision.normal;
            if (existsTranslationPlaneWithNormal(collisionNormal))
                continue;

            auto planeOrigin = _translationPlane->plane.origin;
            auto planePosition = _draggingCollider->getObb().getPositionAt(-collision.normal);
            auto plane = phi::plane(planeOrigin, collision.normal);
            auto translationPlane = createTranslationPlane(plane, planePosition, collision.collider);
            addTranslationPlane(translationPlane);
            addPlaneGrid(translationPlane);
        }
    }

    void planesTranslationInputController::removeDetachedPlanes(vector<sweepCollision> touchs)
    {
        auto translationPlanesCount = _translationPlanes.size();
        for (size_t i = 0; i < translationPlanesCount; ++i)
        {
            auto translationPlane = _translationPlanes[i];

            auto translationPlaneNormal = translationPlane->plane.normal;
            auto touchsSearch = std::find_if(touchs.begin(), touchs.end(),
                [translationPlaneNormal](sweepCollision& c)
            {
                return mathUtils::isClose(glm::dot(translationPlaneNormal, c.normal), 1.0f);
            });

            if (touchsSearch == touchs.end())
            {
                deletePlane(translationPlane);
                removeTranslationPlane(translationPlane);
                i--;
                translationPlanesCount--;
            }
        }
    }

    bool planesTranslationInputController::onMouseDown(mouseEventArgs * e)
    {
        translationInputController::onMouseDown(e);

        if (!_dragging)
            return false;

        auto touchs = findTouchCollisions();
        addPlanesIfNeeded(touchs);

        return true;
    }

    bool planesTranslationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto touchs = findTouchCollisions();
        addPlanesIfNeeded(touchs);
        removeDetachedPlanes(touchs);

        return translationInputController::onMouseMove(e);
    }
}