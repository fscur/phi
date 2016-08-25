#include <precompiled.h>

#include "translationInputController.h"

namespace phi
{
    translationInputController::translationInputController(const vector<node*>* targetNodes) :
        inputController(),
        _targetNodes(targetNodes)
    {
    }

    translationInputController::~translationInputController()
    {
    }

    bool translationInputController::onMouseDown(mouseEventArgs* e)
    {
        return false;
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        return false;
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        return false;
    }
}