#pragma once

#include <phi.h>

#include <core\boxCollider.h>
#include <core\color.h>

#include <rendering\camera.h>
#include <rendering\pickingFramebuffer.h>

#include "inputController.h"
#include "pickingId.h"
#include "nodeTranslator.h"
#include "collisionNodeTranslator.h"

namespace phi
{
    class obbTranslationInputController :
        public inputController
    {
    private:
        camera* _camera;
        nodeTranslator* _nodeTranslator;
        collisionNodeTranslator* _collisionNodeTranslator;
        boxCollider* _draggingCollider;
        node* _draggingRootNode;
        bool _dragging;
        plane _plane;
        vec3 _initialObjectPosition;
        bool _disableCollision;

    private:
        void initializeDragData(node* node);
        void setPlane(plane plane);
        void showPlaneGrid(vec3 position, color color);

    public:
        obbTranslationInputController(camera* camera, nodeTranslator* nodeTranslator);
        ~obbTranslationInputController();

        void setCollisionNodeTranslator(collisionNodeTranslator* value) { _collisionNodeTranslator = value; }

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onKeyDown(keyboardEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;
    };
}