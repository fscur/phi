#include <precompiled.h>
#include "translationService.h"
#include <core\boxCollider.h>
#include <animation\animator.h>

namespace phi
{
    translationService::translationService(const vector<node*>* targetNodes, layer* layer) :
        _targetNodes(targetNodes),
        _layer(layer),
        _camera(layer->getCamera()),
        _translationPlanes(vector<translationPlane*>()),
        _isTranslating(false),
        _planesToRemove(vector<translationPlane*>())
    {
    }

    translationService::~translationService()
    {
    }

    translationPlane* translationService::createTranslationPlane(vec3 origin, vec3 normal)
    {
        auto planeGridNode = new node("plane");
        auto planeTransform = planeGridNode->getTransform();
        planeTransform->setLocalPosition(origin);
        planeTransform->setDirection(normal);

        auto animator = new phi::animator();

        auto planeGrid = new phi::planeGrid();
        planeGrid->setColor(color::blue);
        planeGrid->setLineThickness(8.5f);

        planeGridNode->addComponent(planeGrid);
        planeGridNode->addComponent(animator);

        auto draggingAnimation = new phi::transformAnimation(planeTransform, easingFunctions::easeOutCubic);
        animator->addAnimation(draggingAnimation);

        auto fadeUpdadeFunction = [=](float value)
        {
            planeGrid->setOpacity(value);
        };

        auto fadeInAnimation = new phi::floatAnimation();
        fadeInAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeInAnimation);

        auto fadeOutAnimation = new phi::floatAnimation();
        fadeOutAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeOutAnimation);

        auto clippingPlanesFadeOutAnimation = new phi::floatAnimation();
        animator->addAnimation(clippingPlanesFadeOutAnimation);

        auto gridPlane = plane(origin, normal);

        auto translationPlane = new phi::translationPlane(origin, normal);
        translationPlane->setGridPlane(gridPlane);
        translationPlane->setCollidee(nullptr);
        translationPlane->setCollider(nullptr);
        translationPlane->setPlaneGridNode(planeGridNode);
        translationPlane->setDraggingAnimation(draggingAnimation);
        translationPlane->setFadeInAnimation(fadeInAnimation);
        translationPlane->setFadeOutAnimation(fadeOutAnimation);
        translationPlane->setClippingPlanesFadeAnimation(clippingPlanesFadeOutAnimation);

        return translationPlane;
    }

    vec3 translationService::getClosestAxisNormal(vec3 direction)
    {
        vec3 axisNormals[] =
        {
            vec3(1.0f, 0.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, 1.0f)
        };

        auto maxDot = -2.0f;
        auto maxNormal = vec3();
        for (uint_fast8_t i = 0; i < 3; ++i)
        {
            auto dot = glm::dot(direction, axisNormals[i]);
            if (dot > maxDot)
            {
                maxDot = dot;
                maxNormal = axisNormals[i];
            }
        }

        assert(maxNormal != vec3());

        return maxNormal;
    }

    void translationService::startTranslation(ivec2 mousePosition)
    {
        auto viewDirection = -_camera->screenPointToRay(mousePosition.x, mousePosition.y).getDirection();
        auto planeNormal = getClosestAxisNormal(viewDirection);

        auto minDot = std::numeric_limits<float>().max();
        auto planeOrigin = vec3();
        for (auto& targetNode : *_targetNodes)
        {
            auto collider = targetNode->getComponent<boxCollider>();
            assert(collider != nullptr);

            auto obb = collider->getObb();
            auto obbPosition = obb.getPositionAt(-planeNormal);

            auto dot = glm::dot(obbPosition, planeNormal);
            if (dot < minDot)
            {
                minDot = dot;
                planeOrigin = obbPosition;
            }
        }

        auto translationPlane = createTranslationPlane(planeOrigin, planeNormal);
        _layer->add(translationPlane->getPlaneGridNode());
        _translationPlanes.push_back(translationPlane);
        translationPlane->showGrid();

        _isTranslating = true;
    }

    void translationService::endTranslation()
    {
        _isTranslating = false;

        for (auto& translationPlane : _translationPlanes)
            enqueuePlaneForRemoval(translationPlane);

        _translationPlanes.clear();
    }

    void translationService::cancelTranslation()
    {
    }

    void translationService::enqueuePlaneForRemoval(translationPlane* planeToRemove)
    {
        auto fadeOutAnimationEnded = [=]
        {
            _planesToRemove.push_back(planeToRemove);
        };

        planeToRemove->fadeGridOpacityOut(fadeOutAnimationEnded);
    }

    void translationService::update()
    {
        if (!_isTranslating)
            return;

        for (auto& translationPlane : _planesToRemove)
        {
            auto planeGridNode = translationPlane->getPlaneGridNode();
            planeGridNode->getParent()->removeChild(planeGridNode);
            safeDelete(translationPlane);
        }

        _planesToRemove.clear();

        // Remove not valid anymore translation planes:
        // TODO: create valid plane if removed all
        //vector<translationPlane*> translationPlanesToRemove;

        //for (auto& translationPlane : _translationPlanes)
        //{
        //    if (!canTranslateAt(translationPlane->getMousePlane().normal))
        //        translationPlanesToRemove.push_back(translationPlane);
        //}

        //for (auto& planeToRemove : translationPlanesToRemove)
        //{
        //    removeTranslationPlane(planeToRemove);
        //    removeClippingPlanes(planeToRemove);
        //}
    }
}