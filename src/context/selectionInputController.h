#pragma once
#include <phi.h>
#include "contextApi.h"

#include <animation\floatAnimator.h>

#include <application\commandsManager.h>

#include "meshLayerBehaviour.h"
#include "inputController.h"

namespace phi
{
    class selectionInputController :
        public inputController
    {
    private:
        meshLayerBehaviour* _meshLayerBehaviour;
        commandsManager* _commandsManager;
        bool _isCtrlPressed;

    private:
        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onKeyDown(keyboardEventArgs* e) override;
        virtual void onKeyUp(keyboardEventArgs* e) override;

    public:
        CONTEXT_API selectionInputController(meshLayerBehaviour* meshLayerBehaviour, commandsManager* commandsManager);
        virtual ~selectionInputController() {}
    };
}