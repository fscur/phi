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
    public:
        rotationInputController(commandsManager* commandsManager, const vector<node*>* targetNodes, layer* layer, physicsWorld* physicsWorld);
        ~rotationInputController();

        bool isEnabled() { return _isEnabled; }
        void enable() { _isEnabled = true; }
        void disable() { _isEnabled = false; }

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
        bool onMouseClick(mouseEventArgs* e) override;
        bool onKeyDown(keyboardEventArgs* e) override;
        bool onKeyUp(keyboardEventArgs* e) override;

        bool update() override;
        void cancel() override;

    private:
        bool canStartRotation(mouseEventArgs* e, node*& clickedNode);
        void startRotation();
        void pushRotateCommands();

    public:
        eventHandler<> rotationStarted;
        eventHandler<> rotationEnded;

    private:
        commandsManager* _commandsManager;
        const vector<node*>* _targetNodes;
        rotationService* _rotationService;
        unordered_map<node*, vec3> _originalPositions;
        unordered_map<node*, quat> _originalOrientations;
        bool _isMouseHidden;
        bool _isEnabled;
        bool _isWaitingToStart;
        double _startTimer;
        ivec2 _startMousePosition;
        node* _startClickedNode;
    };
}