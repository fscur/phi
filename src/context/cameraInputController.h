#pragma once
#include <phi.h>

#include <rendering\camera.h>

#include "contextApi.h"
#include "inputController.h"
#include "zoomInputController.h"
#include "rotationInputController.h"
#include "panInputController.h"

namespace phi
{
    class cameraInputController :
        public inputController
    {
    private:
        zoomInputController _zoomController;
        rotationInputController _rotationController;
        panInputController _panController;

    private:
        virtual bool onKeyDown(keyboardEventArgs* e) override;
        virtual bool onKeyUp(keyboardEventArgs* e) override;

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
        virtual bool onMouseWheel(mouseEventArgs* e) override;

    public:
        CONTEXT_API cameraInputController(camera* camera);
        CONTEXT_API virtual ~cameraInputController() {}

        virtual bool update() override;
    };
}