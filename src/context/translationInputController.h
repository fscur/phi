#pragma once

#include <phi.h>

#include <core\node.h>

#include "inputController.h"

namespace phi
{
    class translationInputController :
        public inputController
    {
    private:
        const vector<node*>* _targetNodes;

    public:
        translationInputController(const vector<node*>* targetNodes);
        ~translationInputController();

        bool onMouseDown(mouseEventArgs* e) override;
        bool onMouseMove(mouseEventArgs* e) override;
        bool onMouseUp(mouseEventArgs* e) override;
    };
}