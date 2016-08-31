#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\boxCollider.h>
#include <core\node.h>
#include <core\plane.h>
#include <core\clippingPlane.h>
#include <animation\transformAnimation.h>
#include <animation\floatAnimation.h>
#include <core\planeGrid.h>

namespace phi
{
    class translationPlane
    {
    private:
        plane _plane;
        clippingPlane* _clippingPlane;
        node* _planeGridNode;
        planeGrid* _planeGridComponent;
        boxCollider* _collidee;
        boxCollider* _collider;
        transformAnimation* _draggingAnimation;
        floatAnimation* _fadeInAnimation;
        floatAnimation* _fadeOutAnimation;
        floatAnimation* _clippingPlanesFadeAnimation;
        float _extinctionFactor;
        bool _isHiding;

    public:
        LAYERS_API translationPlane(vec3 origin, vec3 normal);
        
        plane getPlane() const { return _plane; }
        clippingPlane* getClippingPlane() const { return _clippingPlane; }
        node* getPlaneGridNode() const { return _planeGridNode; }
        boxCollider* getCollidee() const { return _collidee; }
        boxCollider* getCollider() const { return _collider; }
        transformAnimation* getDraggingAnimation() const { return _draggingAnimation; }
        floatAnimation* getFadeInAnimation() const { return _fadeInAnimation; }
        floatAnimation* getFadeOutAnimation() const { return _fadeOutAnimation; }
        floatAnimation* getClippingPlanesFadeAnimation() const { return _clippingPlanesFadeAnimation; }
        float getExtinctionfactor() const { return _extinctionFactor; }

        void setPlane(plane value) { _plane = value; }
        void setClippingPlane(clippingPlane* value) { _clippingPlane = value; }
        void setPlaneGridNode(node* value);

        void setCollidee(boxCollider* value) { _collidee = value; }
        void setCollider(boxCollider* value) { _collider = value; }
        void setDraggingAnimation(transformAnimation* value) { _draggingAnimation = value; }
        void setFadeInAnimation(floatAnimation* value) { _fadeInAnimation = value; }
        void setClippingPlanesFadeAnimation(floatAnimation* value) { _clippingPlanesFadeAnimation = value; }
        void setExtinctionFactor(float value) { _extinctionFactor = value; }

        LAYERS_API void setFadeOutAnimation(floatAnimation* value);

        LAYERS_API planeGrid* getPlaneGridComponent() { return _planeGridNode->getComponent<planeGrid>(); }

        LAYERS_API void showGrid();
        LAYERS_API void hideGrid();

        LAYERS_API void fadeGridOpacityIn(std::function<void(void)> fadeInEndedFunction);
        LAYERS_API void fadeGridOpacityOut(std::function<void(void)> fadeOutEndedFunction);
        
        LAYERS_API void updatePlaneGridVisibility(float visibility);
        LAYERS_API void animatePlaneGridPosition(vec3 position);
    };
}