#ifndef _PHI_UI_SYSTEM_H_
#define _PHI_UI_SYSTEM_H_

#include "control.h"
#include "size.h"
#include "cursor.h"
#include "quadRenderer2D.h"

namespace phi
{
    struct uiSystemInfo
    {
    public:
        std::string applicationPath;
        size<GLuint> size;
    };

    class uiSystem
    {
    private:
        static uiSystem* _instance;
        std::vector<control*> _controls;
        uiSystemInfo _info;
        eventHandler<controlEventArgs>* _controlGotFocus;
        eventHandler<controlEventArgs>* _controlLostFocus;
        cursor* _cursor;
        quadRenderer2D* _cursorRenderer;

    private:
        uiSystem();
        void notifyControlGotFocus(controlEventArgs e);
        void notifyControlLostFocus(controlEventArgs e);
        void controlGotFocus(controlEventArgs e);
        void controlMouseLeave(mouseEventArgs e);
        void inputMouseMove(mouseEventArgs e);
        void renderCursor();

    public:
        UI_API static uiSystem* get();
        UI_API void init(uiSystemInfo info);
        UI_API uiSystemInfo getInfo() { return _info; }
        UI_API eventHandler<controlEventArgs>* getControlGotFocus() { return _controlGotFocus; }
        UI_API eventHandler<controlEventArgs>* getControlLostFocus() { return _controlLostFocus; }
        UI_API void setCursor(cursor* value);
        UI_API void resize(size<GLuint> value);

        UI_API void addControl(control* control);
        UI_API void render();
    };
}

#endif