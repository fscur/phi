#pragma once

#include <phi.h>

#include <core\boxCollider.h>
#include <core\color.h>

#include <rendering\camera.h>
#include <rendering\pickingFramebuffer.h>

#include <animation\animator.h>
#include <animation\animation.h>
#include <animation\floatAnimation.h>
#include <animation\transformAnimation.h>

#include "collisionNodeTranslator.h"
#include "inputController.h"
#include "layer.h"
#include "pickingId.h"
#include "translationPlane.h"

namespace phi
{
    class translationInputController :
        public inputController
    {
    protected:
        camera* _camera;
        collisionNodeTranslator* _collisionNodeTranslator;
        layer* _planesLayer;
        bool _dragging;
        node* _draggingRootNode;
        boxCollider* _draggingCollider;
        translationPlane* _defaultTranslationPlane;
        vec3 _initialObjectPosition;
        bool _disableCollision;
        ivec2 _lastMousePosition;
        vector<sweepCollision>* _lastTranslationTouchs;

    private:
        void setNodeToTranslate(node* node);
        void initializeNodeTranslators();
        void endNodeTranslators();

    protected:
        void setupTranslationPlane(translationPlane* translationPlane);
        translationPlane* createTranslationPlane(plane plane, vec3 position, boxCollider* collider, color color = color::fromRGBA(0.5f, 0.6f, 0.7f, 1.0f));
        void deletePlane(translationPlane* translationPlane);
        vec3 getTranslationOffset(ivec2 mousePosition, translationPlane* translationPlane);
        void translateNode(vec3 offset);
        void translatePlaneGrid(translationPlane* translationPlane);

    public:
        translationInputController(camera* camera, layer* planesLayer);
        ~translationInputController();

        void setCollisionNodeTranslator(collisionNodeTranslator* value) { _collisionNodeTranslator = value; }

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onKeyDown(keyboardEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;
    };
}