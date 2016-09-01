#include <precompiled.h>
#include "translationPlane.h"
#include <cassert>
#include <core/string.h>
#include <animation/easingFunctions.h>
namespace phi
{
    translationPlane::translationPlane(vec3 origin, vec3 normal) :
        _plane(plane()),
        _collidee(nullptr),
        _planeGridNode(nullptr),
        _draggingAnimation(nullptr),
        _fadeInAnimation(nullptr),
        _fadeOutAnimation(nullptr)
    {
    }

    void translationPlane::setPlaneGridNode(node * value)
    {
        _planeGridNode = value;
        _planeGridComponent = _planeGridNode->getComponent<planeGrid>();
        assert(_planeGridComponent);
    }

    void translationPlane::setFadeOutAnimation(floatAnimation* value)
    {
        _fadeOutAnimation = value;
    }

    void translationPlane::showGrid()
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeInAnimation != nullptr);

        auto planeGrid = _planeGridNode->getComponent<phi::planeGrid>();

        if (planeGrid->isVisible())
            return;

        planeGrid->show();

        _fadeInAnimation->start(planeGrid->getOpacity(), 1.0f, 0.4);
    }

    void translationPlane::hideGrid()
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);

        auto planeGrid = _planeGridNode->getComponent<phi::planeGrid>();

        if (!planeGrid->isVisible())
            return;

        planeGrid->hide();

        /*auto hidePlaneGridFunc = [&] 
        {
            auto planeGrid = _planeGridNode->getComponent<phi::planeGrid>();
            planeGrid->hide();
        };

        _fadeOutAnimation->start(planeGrid->getOpacity(), 0.0f, 0.8f, hidePlaneGridFunc);*/
    }

    void translationPlane::fadeGridOpacityIn(std::function<void(void)> fadeInEndedFunction)
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);

        auto planeGrid = _planeGridNode->getComponent<phi::planeGrid>();
        _fadeOutAnimation->start(planeGrid->getOpacity(), 1.0f, 0.8f, fadeInEndedFunction);
    }

    void translationPlane::fadeGridOpacityOut(std::function<void(void)> fadeOutEndedFunction)
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);
        
        auto planeGrid = _planeGridNode->getComponent<phi::planeGrid>();
        _fadeOutAnimation->start(planeGrid->getOpacity(), 0.0f, 0.8f, fadeOutEndedFunction);
    }

    void translationPlane::updatePlaneGridVisibility(float visibility)
    {
        auto min = _extinctionFactor;
        auto v = (visibility - min) / (1.0f - min);
        auto planeGridVisibility = static_cast<float>(glm::pow(1.0f - v, 4.0));
        
        _planeGridComponent->setVisibilityFactor(planeGridVisibility);
    }

    void translationPlane::animatePlaneGridPosition(vec3 position)
    {
        auto planeTransform = _planeGridNode->getTransform();

        auto fromPlaneTransform = new transform();
        auto fromPosition = planeTransform->getLocalPosition();

        fromPlaneTransform->setLocalPosition(fromPosition);

        auto toPlaneTransform = new transform();
        toPlaneTransform->setLocalPosition(position);

        _draggingAnimation->start(fromPlaneTransform, toPlaneTransform, 0.33);
    }
}