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
#include "uiRepository.h"
#include "controlEventArgs.h"
#include "toolTip.h"

namespace phi
{
    class control : public eventSender
    {
    private:
        static toolTip* _toolTip;
        bool _isMouseOver;
        bool _isFocused;
        mouseEventHandler* _mouseEnter;
        mouseEventHandler* _mouseLeave;
        eventHandler<controlEventArgs>* _gotFocus;
        eventHandler<controlEventArgs>* _lostFocus;
        eventHandler<controlEventArgs>* _addedChild;
        eventHandler<controlEventArgs>* _removedChild;
        std::string _toolTipText;
        float _mouseStillTime;
        bool _renderToolTip;
        std::vector<control*> _children;

        void notifyGotFocus(controlEventArgs e);
        void notifyLostFocus(controlEventArgs e);

    protected:
        int _x;
        int _y;
        float _zIndex;
        size<GLuint> _size;
        size<GLuint> _viewportSize;

    public:
        UI_API control(size<GLuint> viewportSize);
        UI_API ~control();

        static void init(size<GLuint> viewportSize);

        UI_API void addChild(control* child);
        UI_API void removeChild(control* child);

        UI_API mouseEventHandler* getMouseEnter() { return _mouseEnter; }
        UI_API mouseEventHandler* getMouseLeave() { return _mouseLeave; }
        UI_API eventHandler<controlEventArgs>* getGotFocus() { return _gotFocus; }
        UI_API eventHandler<controlEventArgs>* getLostFocus() { return _lostFocus; }
        UI_API eventHandler<controlEventArgs>* getAddedChild() { return _addedChild; }
        UI_API eventHandler<controlEventArgs>* getRemovedChild() { return _removedChild; }

        UI_API virtual int getX() { return _x; }
        UI_API virtual int getY() { return _y; }
        UI_API virtual float getZIndex() { return _zIndex; }
        UI_API virtual size<GLuint> getSize() { return _size; }
        UI_API virtual bool getIsMouseOver() { return _isMouseOver; }
        UI_API virtual bool getIsFocused() { return _isFocused; }
        UI_API virtual size<GLuint> getViewportSize() { return _viewportSize; }
        UI_API std::string getToolTipText() const { return _toolTipText; }
        UI_API std::vector<control*> getChildren() { return _children; }

        UI_API virtual void setX(int value) { _x = value; }
        UI_API virtual void setY(int value) { _y = value; }
        UI_API virtual void setZIndex(float value) { _zIndex = value; }
        UI_API virtual void setSize(size<GLuint> value) { _size = value; }
        UI_API virtual void setViewportSize(size<GLuint> value) { _viewportSize = value; }
        UI_API virtual void setIsFocused(bool value);
        UI_API void setToolTipText(const std::string value) { _toolTipText = value; }

        UI_API virtual bool isPointInside(int x, int y);
        UI_API virtual void update();
        UI_API void render();

        UI_API void virtual onRender();
        UI_API void virtual onMouseDown(mouseEventArgs* e) {}
        UI_API void virtual onMouseUp(mouseEventArgs* e) {}
        UI_API void virtual onMouseMove(mouseEventArgs* e) {}
        UI_API void virtual onMouseWheel(mouseEventArgs* e) {}
        UI_API void virtual onMouseEnter(mouseEventArgs* e) {}
        UI_API void virtual onMouseLeave(mouseEventArgs* e) {}
        UI_API void virtual onKeyDown(keyboardEventArgs e) {}
        UI_API void virtual onKeyUp(keyboardEventArgs e) {}

        void notifyMouseEnter(mouseEventArgs* e);
        void notifyMouseLeave(mouseEventArgs* e);
        void notifyMouseDown(mouseEventArgs* e);
        void notifyMouseUp(mouseEventArgs* e);
        void notifyMouseMove(mouseEventArgs* e);
        void notifyMouseWheel(mouseEventArgs* e);
        void notifyKeyDown(keyboardEventArgs e);
        void notifyKeyUp(keyboardEventArgs e);
    };
}

#endif