
#pragma once
#include <phi.h>

#include <core\size.h>

#include <rendering\gl.h>
#include <rendering\font.h>

#include "uiRenderer.h"

#include "cursor.h"
#include "control.h"

namespace phi
{
    struct uiSystemInfo
    {
    public:
        gl* gl;
        string applicationPath;
        string resourcesPath;
        sizef size;
    };

    struct controlEventTokens
    {
        eventToken gotFocus;
        eventToken mouseLeave;
        eventToken addedChild;
        eventToken removedChild;
        eventToken propertyChanged;
    };

    class uiSystem
    {
    private:
        static uiSystem* _instance;
        vector<control*> _controls;
        vector<control*> _rootControls;
        map<control*, controlEventTokens> _controlsEventTokens;
        uiSystemInfo _info;
        eventHandler<controlEventArgs>* _controlGotFocus;
        eventHandler<controlEventArgs>* _controlLostFocus;
        vector<std::pair<control*, bool>> _controlsToAdd;
        vector<control*> _controlsToRemove;

        gl* _gl;
        font* _font;
        uiRenderer* _uiRenderer;

    private:
        uiSystem();
        ~uiSystem();

        void addControlToLists(control* control, bool root);
        void removeControlFromLists(control* control);

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
        void initRenderer(float width, float height);
        void controlProperyChanged(control* sender);

    public:
        UI_API static uiSystem* get();
        UI_API void init(uiSystemInfo info);
        UI_API void setCursor(cursor* value);
        UI_API void resize(sizef value);

        UI_API void addControl(control* control);
        UI_API void removeControl(control* control);
        UI_API void update();
        UI_API void render();

        uiSystemInfo getInfo() { return _info; }
        eventHandler<controlEventArgs>* getControlGotFocus() { return _controlGotFocus; }
        eventHandler<controlEventArgs>* getControlLostFocus() { return _controlLostFocus; }
    };
}