#ifndef _PHI_CONTROL_H_
#define _PHI_CONTROL_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include "ui.h"
#include "input.h"
#include "globals.h"
#include "size.h"
#include "uiSystem.h"

namespace phi
{
    class control
    {
    private:
        bool _isMouseOver;
        mouseEventHandler* _mouseEnter;
        mouseEventHandler* _mouseLeave;

        void notifyMouseEnter(mouseEventArgs e);
        void notifyMouseLeave(mouseEventArgs e);

    protected:
        int _x;
        int _y;
        size<GLuint> _size;
		size<GLuint> _viewportSize;

        void inputMouseDown(mouseEventArgs e);
        void inputMouseUp(mouseEventArgs e);
        void inputMouseMove(mouseEventArgs e);
        void inputMouseWheel(mouseEventArgs e);

        void virtual onMouseDown(mouseEventArgs e) {}
        void virtual onMouseUp(mouseEventArgs e) {}
        void virtual onMouseMove(mouseEventArgs e) {}
        void virtual onMouseWheel(mouseEventArgs e) {}
        void virtual onMouseEnter(mouseEventArgs e) {}
        void virtual onMouseLeave(mouseEventArgs e) {}

    public:
        UI_API control();
        UI_API ~control();

        UI_API mouseEventHandler* getMouseEnter() { return _mouseEnter; }
        UI_API mouseEventHandler* getMouseLeave() { return _mouseLeave; }

        UI_API virtual int getX() { return _x; }
        UI_API virtual int getY() { return _y; }
        UI_API virtual size<GLuint> getSize() { return _size; }
        UI_API virtual bool getIsMouseOver() { return _isMouseOver; }
		UI_API virtual size<GLuint> getViewportSize() { return _viewportSize; }

        UI_API virtual void setX(int value) { _x = value; }
        UI_API virtual void setY(int value) { _y = value; }
		UI_API virtual void setSize(size<GLuint> value) { _size = value; }
		UI_API virtual void setViewportSize(size<GLuint> value) { _viewportSize = value; }

        UI_API virtual void render() = 0;
    };
}

#endif