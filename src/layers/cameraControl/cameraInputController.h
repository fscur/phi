#pragma once
#include <phi.h>
#include "..\layersApi.h"

#include <rendering\camera.h>

#include <input\inputController.h>
#include "cameraZoomInputController.h"
#include "cameraRotationInputController.h"
#include "cameraPanInputController.h"

namespace phi
{
    class cameraInputController :
        public inputController
    {
    private:
        cameraZoomInputController _zoomController;
        cameraRotationInputController _rotationController;
        cameraPanInputController _panController;

    private:
        virtual bool onKeyDown(keyboardEventArgs* e) override;
        virtual bool onKeyUp(keyboardEventArgs* e) override;

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
        virtual bool onBeginMouseWheel(mouseEventArgs* e) override;
        virtual bool onMouseWheel(mouseEventArgs* e) override;
        virtual bool onEndMouseWheel(mouseEventArgs* e) override;

    public:
        LAYERS_API cameraInputController(camera* camera);
        LAYERS_API virtual ~cameraInputController() {}

        virtual bool update() override;
    };
}