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

#include <animation\animator.h>
#include <animation\animation.h>
#include <animation\floatAnimation.h>
#include <animation\transformAnimation.h>

namespace phi
{
    class obbTranslationInputController :
        public inputController
    {
    private:
        camera* _camera;
        nodeTranslator* _nodeTranslator;
        collisionNodeTranslator* _collisionNodeTranslator;
        layer* _planesLayer;
        image* _planeImage;

        bool _dragging;
        boxCollider* _draggingCollider;
        node* _draggingRootNode;
        plane _plane;
        vec3 _initialObjectPosition;
        bool _disableCollision;
        map<node*, transformAnimation*> _planeNodesAnimations;

    private:
        void initializeDragData(node* node);
        void setPlane(plane plane);
        void clearPlaneGrids();
        void addPlaneGrid(vec3 position, color color);
        void updatePlaneGrids();

    public:
        obbTranslationInputController(camera* camera, layer* planesLayer);
        ~obbTranslationInputController();

        void setCollisionNodeTranslator(collisionNodeTranslator* value) { _collisionNodeTranslator = value; }

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onKeyDown(keyboardEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;
    };
}