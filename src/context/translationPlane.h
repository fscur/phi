#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\boxCollider.h>
#include <core\node.h>
#include <core\plane.h>
#include <core\clippingPlane.h>
#include <animation\transformAnimation.h>
#include <animation\floatAnimation.h>
#include <ui\planeGrid.h>

namespace phi
{
    class translationPlane
    {
    private:
        plane _gridPlane;
        plane _mousePlane;
        clippingPlane* _clippingPlane;
        node* _planeGridNode;
        boxCollider* _collidee;
        boxCollider* _collider;
        transformAnimation* _draggingAnimation;
        floatAnimation* _fadeInAnimation;
        floatAnimation* _fadeOutAnimation;
        floatAnimation* _clippingPlanesFadeOutAnimation;

    public:
        CONTEXT_API translationPlane(phi::plane plane);
        
        plane getGridPlane() const { return _gridPlane; }
        plane getMousePlane() const { return _mousePlane; }
        clippingPlane* getClippingPlane() const { return _clippingPlane; }
        node* getPlaneGridNode() const { return _planeGridNode; }
        boxCollider* getCollidee() const { return _collidee; }
        boxCollider* getCollider() const { return _collider; }
        transformAnimation* getDraggingAnimation() const { return _draggingAnimation; }
        floatAnimation* getFadeInAnimation() const { return _fadeInAnimation; }
        floatAnimation* getFadeOutAnimation() const { return _fadeOutAnimation; }
        floatAnimation* getClippingPlanesFadeOutAnimation() const { return _clippingPlanesFadeOutAnimation; }

        void setGridPlane(plane value) { _gridPlane = value; }
        void setMousePlane(plane value) { _mousePlane = value; }
        void setClippingPlane(clippingPlane* value) { _clippingPlane = value; }
        void setPlaneGridNode(node* value) { _planeGridNode = value; }
        void setCollidee(boxCollider* value) { _collidee = value; }
        void setCollider(boxCollider* value) { _collider = value; }
        void setDraggingAnimation(transformAnimation* value) { _draggingAnimation = value; }
        void setFadeInAnimation(floatAnimation* value) { _fadeInAnimation = value; }
        void setClippingPlanesFadeOutAnimation(floatAnimation* value) { _clippingPlanesFadeOutAnimation = value; }

        CONTEXT_API void setFadeOutAnimation(floatAnimation* value);

        CONTEXT_API planeGrid* getPlaneGridComponent() { return _planeGridNode->getComponent<planeGrid>(); }

        CONTEXT_API void showGrid();
        CONTEXT_API void hideGrid();
    };
}