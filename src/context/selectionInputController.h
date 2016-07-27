#pragma once
#include <phi.h>
#include "contextApi.h"

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

    private:
        virtual bool onMouseDown(mouseEventArgs* e) override;

    public:
        CONTEXT_API selectionInputController(meshLayerBehaviour* meshLayerBehaviour, commandsManager* commandsManager);
        virtual ~selectionInputController() {}
    };
}