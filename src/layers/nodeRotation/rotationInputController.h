#pragma once

#include <phi.h>

#include <core/node.h>
#include <input/inputController.h>
#include <physics/physicsWorld.h>
#include <rendering/camera.h>
#include <application/commandsManager.h>
#include <context/layer.h>
#include "rotationService.h"
#include "rotationPlane.h"

namespace phi
{
    class rotationInputController :
        public inputController
    {
    private:
        commandsManager* _commandsManager;
        const vector<node*>* _targetNodes;
        rotationService* _rotationService;
        unordered_map<node*, vec3> _originalPositions;
        bool _isMouseHidden;

    private:
        bool canStartRotation(mouseEventArgs* e);
        void pushRotateCommands();

    public:
        rotationInputController(commandsManager* commandsManager, const vector<node*>* targetNodes, layer* layer, physicsWorld* physicsWorld);
        ~rotationInputController();

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onMouseClick(mouseEventArgs* e) override;
        bool onKeyDown(keyboardEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;

        bool update() override;
        void cancel() override;
    };
}