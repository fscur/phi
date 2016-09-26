#include <precompiled.h>
#include "rotationPlane.h"
#include <cassert>
#include <core/string.h>

namespace phi
{
    rotationPlane::rotationPlane(plane plane) :
        _plane(plane),
        _clippingPlane(new clippingPlane(plane)),
        _collidee(nullptr),
        _rotationPlaneGridNode(nullptr),
        _draggingAnimation(nullptr),
        _fadeInAnimation(nullptr),
        _fadeOutAnimation(nullptr)
    {
    }

    rotationPlane::~rotationPlane()
    {
        safeDelete(_rotationPlaneGridNode);
        safeDelete(_draggingAnimation);
        safeDelete(_fadeInAnimation);
        safeDelete(_fadeOutAnimation);
        safeDelete(_clippingPlanesFadeAnimation);
    }

    void rotationPlane::setPlaneGridNode(node* value)
    {
        _rotationPlaneGridNode = value;
        _rotationPlaneGrid = _rotationPlaneGridNode->getComponent<rotationPlaneGrid>();
        assert(_rotationPlaneGrid);
    }

    void rotationPlane::setFadeOutAnimation(floatAnimation* value)
    {
        _fadeOutAnimation = value;
    }

    void rotationPlane::showGrid()
    {
        assert(_rotationPlaneGridNode != nullptr);
        assert(_fadeInAnimation != nullptr);

        auto rotationPlaneGrid = _rotationPlaneGridNode->getComponent<phi::rotationPlaneGrid>();

        if (rotationPlaneGrid->isVisible())
            return;

        rotationPlaneGrid->show();

        fadeGridOpacityIn({});
    }

    void rotationPlane::hideGrid()
    {
        assert(_rotationPlaneGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);

        auto rotationPlaneGrid = _rotationPlaneGridNode->getComponent<phi::rotationPlaneGrid>();

        if (!rotationPlaneGrid->isVisible())
            return;

        rotationPlaneGrid->hide();

        /*auto hidePlaneGridFunc = [&] 
        {
            auto rotationPlaneGrid = _rotationPlaneGridNode->getComponent<phi::rotationPlaneGrid>();
            rotationPlaneGrid->hide();
        };

        _fadeOutAnimation->start(rotationPlaneGrid->getOpacity(), 0.0f, 0.8f, hidePlaneGridFunc);*/
    }

    void rotationPlane::fadeGridOpacityIn(std::function<void(void)> fadeInEndedFunction)
    {
        assert(_rotationPlaneGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);

        _fadeInAnimation->start(_rotationPlaneGrid->getOpacity(), 1.0f, FADE_IN_ANIMATION_TIME_IN_SECONDS, fadeInEndedFunction);
    }

    void rotationPlane::fadeGridOpacityOut(std::function<void(void)> fadeOutEndedFunction)
    {
        assert(_rotationPlaneGridNode != nullptr);
        assert(_fadeOutAnimation != nullptr);
        
        _fadeOutAnimation->start(_rotationPlaneGrid->getOpacity(), 0.0f, FADE_OUT_ANIMATION_TIME_IN_SECONDS, fadeOutEndedFunction);
    }

    void rotationPlane::updatePlaneGridVisibilityFactor(float visibility)
    {
        auto min = _extinctionFactor;
        auto v = (visibility - min) / (1.0f - min);
        auto rotationPlaneGridVisibilityFactor = static_cast<float>(glm::pow(1.0f - v, PLANE_VISIBILITY_EXTINCTION_FACTOR));
        
        _rotationPlaneGrid->setVisibilityFactor(rotationPlaneGridVisibilityFactor);
    }

    void rotationPlane::animatePlaneGridPosition(vec3 position)
    {
        auto planeTransform = _rotationPlaneGridNode->getTransform();
        auto fromPosition = planeTransform->getLocalPosition();
        
        transform fromPlaneTransform;
        fromPlaneTransform.setLocalPosition(fromPosition);
        
        transform toPlaneTransform;
        toPlaneTransform.setLocalPosition(position);

        _draggingAnimation->start(fromPlaneTransform, toPlaneTransform, 0.33);
    }
}