#pragma once

#include <phi.h>

#include <core/eventHandler.h>

#include "inputApi.h"
#include "keyboardEventArgs.h"
#include "mouseEventArgs.h"

namespace phi
{
    class inputController
    {
    public:
        INPUT_API virtual ~inputController();

        virtual bool onMouseDown(mouseEventArgs* e) { return false; }
        virtual bool onMouseUp(mouseEventArgs* e) { return false; }
        virtual bool onMouseClick(mouseEventArgs* e) { return false; }
        virtual bool onMouseDoubleClick(mouseEventArgs* e) { return false; }
        virtual bool onMouseMove(mouseEventArgs* e) { return false; }
        virtual bool onBeginMouseWheel(mouseEventArgs* e) { return false; }
        virtual bool onMouseWheel(mouseEventArgs* e) { return false; }
        virtual bool onEndMouseWheel(mouseEventArgs* e) { return false; }
        virtual bool onKeyDown(keyboardEventArgs* e) { return false; }
        virtual bool onKeyUp(keyboardEventArgs* e) { return false; }
        virtual bool update() { return false; }
        virtual void cancel() { }

        eventHandler<inputController*>* getRequestControlEvent() const { return _requestControlEvent; }
        eventHandler<inputController*>* getResignControlEvent() const { return _resignControlEvent; }

    protected:
        INPUT_API inputController();
        eventHandler<inputController*>* _requestControlEvent;
        eventHandler<inputController*>* _resignControlEvent;
    };
}