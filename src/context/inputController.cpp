#include "precompiled.h"
#include "inputController.h"

namespace phi
{
    inputController::inputController() :
        _requestControlEvent(new eventHandler<inputController*>()),
        _resignControlEvent(new eventHandler<inputController*>())
    {
    }
}
