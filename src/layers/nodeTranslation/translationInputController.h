#pragma once

#include <phi.h>

#include <core\node.h>
#include <rendering\camera.h>

#include <input\inputController.h>
#include <context\layer.h>
#include "translationPlane.h"
#include "translationService.h"

namespace phi
{
    class translationInputController :
        public inputController
    {
    private:
        commandsManager* _commandsManager;
        translationService* _translationService;
        const vector<node*>* _targetNodes;
        unordered_map<node*, vec3> _originalPositions;

    private:
        bool canStartTranslation(mouseEventArgs* e);
        void pushTranslateCommands();

    public:
        translationInputController(commandsManager* commandsManager, const vector<node*>* targetNodes, layer* layer, physicsWorld* physicsWorld);
        ~translationInputController();

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