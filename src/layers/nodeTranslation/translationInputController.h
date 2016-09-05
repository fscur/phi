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
        translationService* _translationService;

    private:
        bool canStartTranslation(mouseEventArgs* e);

    public:
        translationInputController(const vector<node*>* targetNodes, layer* layer, physicsWorld* physicsWorld);
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