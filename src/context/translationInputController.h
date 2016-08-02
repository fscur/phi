#pragma once

#include <phi.h>

#include <core\boxCollider.h>
#include <core\color.h>

#include <rendering\camera.h>
#include <rendering\pickingFramebuffer.h>

#include "collisionNodeTranslator.h"
#include "inputController.h"
#include "layer.h"
#include "nodeTranslator.h"
#include "pickingId.h"
#include "translationPlane.h"

#include <animation\animator.h>
#include <animation\animation.h>
#include <animation\floatAnimation.h>
#include <animation\transformAnimation.h>

namespace phi
{
    class translationInputController :
        public inputController
    {
    protected:
        camera* _camera;
        nodeTranslator* _nodeTranslator;
        collisionNodeTranslator* _collisionNodeTranslator;
        layer* _planesLayer;
        image* _planeImage;
        bool _dragging;
        node* _draggingRootNode;
        boxCollider* _draggingCollider;
        translationPlane* _translationPlane;
        translationPlane* _defaultTranslationPlane;
        vec3 _initialObjectPosition;
        bool _disableCollision;
        vector<translationPlane*> _translationPlanes;
        vec2 _lastMousePosition;

    private:
        void startDrag(node* node);
        void startNodeTranslators();
        void endNodeTranslators();
        vec3 getTranslationOffset(vec2 mousePosition);
        void translateNode(vec3 offset);
        void translatePlaneGrids();
        void translatePlaneGrid(translationPlane* translationPlane);

    protected:
        void setPlane(translationPlane* translationPlane);
        translationPlane* createTranslationPlane(plane plane, vec3 position, boxCollider* collider);
        void addTranslationPlane(translationPlane* translationPlane);
        void removeTranslationPlane(translationPlane* translationPlane);
        void addPlaneGrid(translationPlane* translationPlane);
        void removePlaneGrid(translationPlane* translationPlane);
        void deletePlane(translationPlane* translationPlane);
        void clearPlanes();
        vec3 castRayToPlane(vec2 screenPosition);

    public:
        translationInputController(camera* camera, layer* planesLayer);
        ~translationInputController();

        void setCollisionNodeTranslator(collisionNodeTranslator* value) { _collisionNodeTranslator = value; }
        vec3 mouseOffsetToWorld(vec2 mousePosition);
        translationPlane* findBestPlaneToDrag(vec3 dragDirection);

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onKeyDown(keyboardEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;
    };
}