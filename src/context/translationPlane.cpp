#include <precompiled.h>
#include "translationPlane.h"
#include <cassert>

namespace phi
{
    translationPlane::translationPlane(phi::plane mousePlane) :
        _mousePlane(mousePlane),
        _gridPlane(plane()),
        _collidee(nullptr),
        _planeGridNode(nullptr),
        _draggingAnimation(nullptr),
        _fadeInAnimation(nullptr),
        _fadeOutAnimation(nullptr)
    {
    }

    void translationPlane::setFadeOutAnimation(floatAnimation* value)
    {
        _fadeOutAnimation = value;

        auto fadeOutAnimationEndedFunction = [&](phi::animation* animation)
        {
            auto planeGrid = _planeGridNode->getComponent<phi::planeGrid>();
            planeGrid->hide();
        };

        _fadeOutAnimation->getAnimationEnded()->assign(fadeOutAnimationEndedFunction);
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
        //planeGrid->setOpacity(0.0);
        _fadeOutAnimation->start(planeGrid->getOpacity(), 0.0f, 0.4);
    }
}