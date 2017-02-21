#pragma once

#include <phi.h>
#include <core/node.h>
#include <rendering/camera.h>
#include <input/inputController.h>
#include "../pickingId.h"
#include "translationPlane.h"
#include "translationService.h"

namespace phi
{
    class translationInputController :
        public inputController
    {
    public:
        translationInputController(commandsManager* commandsManager, const vector<node*>* targetNodes, layer* layer, physicsWorld* physicsWorld);
        ~translationInputController();

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
        bool canStartTranslation(mouseEventArgs* e, node*& clickedNode);
        void startTranslation();
        void pushTranslateCommands();

    public:
        eventHandler<> translationStarted;
        eventHandler<> translationEnded;

    private:
        commandsManager* _commandsManager;
        const vector<node*>* _targetNodes;
        translationService* _translationService;
        unordered_map<node*, vec3> _originalPositions;
        bool _isMouseHidden;
        bool _isEnabled;
        bool _isWaitingToStart;
        double _startTimer;
        ivec2 _startMousePosition;
        node* _startClickedNode;
    };
}