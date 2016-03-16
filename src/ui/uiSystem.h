
#pragma once
#include <phi.h>

#include <core\size.h>

#include <rendering\quadRenderer2D.h>

#include "cursor.h"
#include "control.h"

namespace phi
{
    struct uiSystemInfo
    {
    public:
        std::string applicationPath;
        std::string resourcesPath;
        sizeui size;
    };

    struct controlEventTokens
    {
        eventToken gotFocus;
        eventToken mouseLeave;
        eventToken addedChild;
        eventToken removedChild;
    };

    class uiSystem
    {
    private:
        static uiSystem* _instance;
        std::vector<control*> _controls;
        std::vector<control*> _rootControls;
        std::map<control*, controlEventTokens> _controlsEventTokens;
        uiSystemInfo _info;
        eventHandler<controlEventArgs>* _controlGotFocus;
        eventHandler<controlEventArgs>* _controlLostFocus;
        cursor* _cursor;
        quadRenderer2D* _cursorRenderer;
        std::vector<std::pair<control*, bool>> _controlsToAdd;
        std::vector<control*> _controlsToRemove;

    private:
        uiSystem();

        void addControlToList(control* cntrl, bool root);
        void removeControlFromList(control* cntrl);

        void notifyControlGotFocus(controlEventArgs e);
        void notifyControlLostFocus(controlEventArgs e);
        void controlGotFocus(controlEventArgs e);
        void controlMouseLeave(mouseEventArgs* e);
        void controlAddedChild(controlEventArgs e);
        void controlRemovedChild(controlEventArgs e);
        void inputMouseDown(mouseEventArgs* e);
        void inputMouseUp(mouseEventArgs* e);
        void inputMouseMove(mouseEventArgs* e);
        void inputMouseWheel(mouseEventArgs* e);
        void inputKeyDown(keyboardEventArgs* e);
        void inputKeyUp(keyboardEventArgs* e);
        void dragDropEnded(dragDropEventArgs* e);
        void renderCursor();

    public:
        UI_API static uiSystem* get();
        UI_API void init(uiSystemInfo info);
        UI_API void setCursor(cursor* value);
        UI_API void resize(sizeui value);

        UI_API void addControl(control* control);
        UI_API void removeControl(control* control);
        UI_API void update();
        UI_API void render();

        uiSystemInfo getInfo() { return _info; }
        eventHandler<controlEventArgs>* getControlGotFocus() { return _controlGotFocus; }
        eventHandler<controlEventArgs>* getControlLostFocus() { return _controlLostFocus; }
    };
}