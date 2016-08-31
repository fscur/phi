#pragma once
#include <phi.h>
#include "inputApi.h"
#include <core\eventHandler.h>
#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"

namespace phi
{
    class inputController
    {
    protected:
        INPUT_API inputController();
        eventHandler<inputController*>* _requestControlEvent;
        eventHandler<inputController*>* _resignControlEvent;

    public:
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
        virtual ~inputController() {}

        eventHandler<inputController*>* getRequestControlEvent() const { return _requestControlEvent; }
        eventHandler<inputController*>* getResignControlEvent() const { return _resignControlEvent; }
    };
}