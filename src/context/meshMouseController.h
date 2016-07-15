#pragma once
#include <phi.h>
#include "contextApi.h"

#include <animation\floatAnimator.h>

#include <application\commandsManager.h>

#include "meshLayerBehaviour.h"
#include "iMouseController.h"

namespace phi
{
    class meshMouseController :
        public iMouseController
    {
    private:
        meshLayerBehaviour* _meshLayerBehaviour;
        commandsManager* _commandsManager;
        bool _isCtrlPressed;

    private:
        virtual void onKeyDown(keyboardEventArgs* e) override;
        virtual void onKeyUp(keyboardEventArgs* e) override;

        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseWheel(mouseEventArgs* e) override;

    public:
        CONTEXT_API meshMouseController(meshLayerBehaviour* meshLayerBehaviour, commandsManager* commandsManager);
        virtual ~meshMouseController() {}

        CONTEXT_API virtual void update() override;
    };
}