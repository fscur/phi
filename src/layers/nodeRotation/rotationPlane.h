#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <core\boxCollider.h>
#include <core\node.h>
#include <core\plane.h>
#include <core\clippingPlane.h>
#include <animation\transformAnimation.h>
#include <animation\floatAnimation.h>
#include <core\rotationPlaneGrid.h>

namespace phi
{
    class rotationPlane
    {
    private:
        const float FADE_IN_ANIMATION_TIME_IN_SECONDS = 0.4f;
        const float FADE_OUT_ANIMATION_TIME_IN_SECONDS = 0.6f;
        const float PLANE_VISIBILITY_EXTINCTION_FACTOR = 6.0f;
    private:
        plane _plane;
        clippingPlane* _clippingPlane;
        node* _rotationPlaneGridNode;
        rotationPlaneGrid* _rotationPlaneGrid;
        boxCollider* _collidee;
        boxCollider* _collider;
        transformAnimation* _draggingAnimation;
        floatAnimation* _fadeInAnimation;
        floatAnimation* _fadeOutAnimation;
        floatAnimation* _clippingPlanesFadeAnimation;
        float _extinctionFactor;
        bool _isHiding;

    public:
        LAYERS_API rotationPlane(plane plane);
        LAYERS_API ~rotationPlane();

        plane getPlane() const { return _plane; }
        clippingPlane* getClippingPlane() const { return _clippingPlane; }
        node* getPlaneGridNode() const { return _rotationPlaneGridNode; }
        boxCollider* getCollidee() const { return _collidee; }
        boxCollider* getCollider() const { return _collider; }
        transformAnimation* getDraggingAnimation() const { return _draggingAnimation; }
        floatAnimation* getFadeInAnimation() const { return _fadeInAnimation; }
        floatAnimation* getFadeOutAnimation() const { return _fadeOutAnimation; }
        floatAnimation* getClippingPlanesFadeAnimation() const { return _clippingPlanesFadeAnimation; }
        float getExtinctionfactor() const { return _extinctionFactor; }
        
        void setClippingPlane(clippingPlane* value) { _clippingPlane = value; }
        void setPlaneGridNode(node* value);

        void setCollidee(boxCollider* value) { _collidee = value; }
        void setCollider(boxCollider* value) { _collider = value; }
        void setDraggingAnimation(transformAnimation* value) { _draggingAnimation = value; }
        void setFadeInAnimation(floatAnimation* value) { _fadeInAnimation = value; }
        void setClippingPlanesFadeAnimation(floatAnimation* value) { _clippingPlanesFadeAnimation = value; }
        void setExtinctionFactor(float value) { _extinctionFactor = value; }

        LAYERS_API void setFadeOutAnimation(floatAnimation* value);

        LAYERS_API rotationPlaneGrid* getPlaneGrid() { return _rotationPlaneGrid; }

        LAYERS_API void showGrid();
        LAYERS_API void hideGrid();

        LAYERS_API void fadeGridOpacityIn(std::function<void(void)> fadeInEndedFunction);
        LAYERS_API void fadeGridOpacityOut(std::function<void(void)> fadeOutEndedFunction);
        
        LAYERS_API void updatePlaneGridVisibilityFactor(float visibility);
        LAYERS_API void animatePlaneGridPosition(vec3 position);
    };
}