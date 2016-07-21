#pragma once

#include <phi.h>

#include <core\boxCollider.h>
#include <core\color.h>

#include <rendering\camera.h>
#include <rendering\pickingFramebuffer.h>

#include "inputController.h"
#include "pickingId.h"
#include "transformTranslator.h"

namespace phi
{
    class obbTranslationInputController :
        public inputController
    {
    private:
        camera* _camera;
        transformTranslator* _transformTranslator;
        boxCollider* _draggingCollider;
        node* _draggingRootNode;
        bool _dragging;
        plane _plane;
        vec3 _initialObjectPosition;

    private:
        void initializeDragData(node* node);
        void setPlane(plane plane);
        void showPlaneGrid(vec3 position, color color);

    public:
        obbTranslationInputController(camera* camera);
        obbTranslationInputController(camera* camera, transformTranslator* transformTranslator);
        ~obbTranslationInputController();

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
    };
}