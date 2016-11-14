#include <precompiled.h>
#include "translationPlane.h"
#include <cassert>
#include <core/string.h>

namespace phi
{
    translationPlane::translationPlane(plane plane) :
        _plane(plane),
        _clippingPlane(new clippingPlane(plane)),
        _collidee(nullptr),
        _planeGridNode(nullptr),
        _draggingAnimation(nullptr),
        _fadeInAnimation(nullptr),
        _fadeOutAnimation(nullptr)
    {
    }

    translationPlane::~translationPlane()
    {
        safeDelete(_planeGridNode);
        safeDelete(_draggingAnimation);
        safeDelete(_fadeInAnimation);
        safeDelete(_fadeOutAnimation);
        safeDelete(_clippingPlanesFadeAnimation);
    }

    void translationPlane::setPlaneGridNode(node* value)
    {
        _planeGridNode = value;
        _planeGrid = _planeGridNode->getComponent<translationPlaneGrid>();
        assert(_planeGrid);
    }

    void translationPlane::setFadeOutAnimation(floatAnimation* value)
    {
        _fadeOutAnimation = value;
    }

    void translationPlane::showGrid()
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeInAnimation != nullptr);

        auto planeGrid = _planeGridNode->getComponent<phi::translationPlaneGrid>();

        if (planeGrid->isVisible())
            return;

        planeGrid->show();

        fadeGridOpacityIn({});
    }

    void translationPlane::hideGrid()
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);

        auto planeGrid = _planeGridNode->getComponent<phi::translationPlaneGrid>();

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

        _fadeInAnimation->start(_planeGrid->getOpacity(), 1.0f, FADE_IN_ANIMATION_TIME_IN_SECONDS, fadeInEndedFunction);
    }

    void translationPlane::fadeGridOpacityOut(std::function<void(void)> fadeOutEndedFunction)
    {
        assert(_planeGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);
        
        _fadeOutAnimation->start(_planeGrid->getOpacity(), 0.0f, FADE_OUT_ANIMATION_TIME_IN_SECONDS, fadeOutEndedFunction);
    }

    void translationPlane::updatePlaneGridVisibilityFactor(float visibility)
    {
        auto min = _extinctionFactor;
        auto v = (visibility - min) / (1.0f - min);
        auto planeGridVisibilityFactor = static_cast<float>(glm::pow(1.0f - v, PLANE_VISIBILITY_EXTINCTION_FACTOR));
        
        _planeGrid->setVisibilityFactor(planeGridVisibilityFactor);
    }

    void translationPlane::animatePlaneGridPosition(vec3 position)
    {
        auto planeTransform = _planeGridNode->getTransform();
        auto fromPosition = planeTransform->getLocalPosition();
        
        transform fromPlaneTransform;
        fromPlaneTransform.setLocalPosition(fromPosition);
        
        transform toPlaneTransform;
        toPlaneTransform.setLocalPosition(position);

        _draggingAnimation->start(fromPlaneTransform, toPlaneTransform, 0.33);
    }
}