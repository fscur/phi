#ifndef _PHI_CONTROL_H_
#define _PHI_CONTROL_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include "phi/ui/ui.h"
#include "phi/core/input.h"
#include "phi/core/globals.h"
#include "phi/core/size.h"
#include "phi/ui/uiRepository.h"
#include "phi/ui/controlEventArgs.h"
#include "phi/ui/toolTip.h"
#include "phi/ui/scissorStack.h"
#include "phi/ui/dragDropController.h"

namespace phi
{
    class control : public eventSender
    {
    private:
        static toolTip* _toolTip;
        bool _isMouseOver;
        bool _isFocused;
        bool _isTopMost;
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
        std::string _dragData;
        texture* _dragTexture;
        bool _clickedOver;

        void resetToolTip();
        void notifyGotFocus(controlEventArgs e);
        void notifyLostFocus(controlEventArgs e);

    protected:
        static scissorStack* controlsScissors;
        int _x;
        int _y;
        float _zIndex;
        sizef _size;
        sizef _viewportSize;

    public:
        UI_API control(sizef viewportSize);
        UI_API ~control();

        static void init(sizef viewportSize);

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
        UI_API virtual sizef getSize() { return _size; }
        UI_API virtual bool getIsMouseOver() { return _isMouseOver; }
        UI_API virtual bool getIsFocused() { return _isFocused; }
        UI_API virtual bool getIsTopMost() { return _isTopMost; }
        UI_API virtual sizef getViewportSize() { return _viewportSize; }
        UI_API std::string getToolTipText() const { return _toolTipText; }
        UI_API std::vector<control*> getChildren() { return _children; }
        UI_API std::string getDragData() { return _dragData; }
        UI_API texture* getDragTexture() { return _dragTexture; }

        UI_API virtual void setX(int value) { _x = value; resetToolTip(); }
        UI_API virtual void setY(int value) { _y = value; resetToolTip(); }
        UI_API virtual void setZIndex(float value) { _zIndex = value; }
        UI_API virtual void setSize(sizef value) { _size = value; }
        UI_API virtual void setViewportSize(sizef value) { _viewportSize = value; }
        UI_API virtual void setIsFocused(bool value);
        UI_API virtual void setIsTopMost(bool value) { _isTopMost = value; }
        UI_API void setToolTipText(const std::string value) { _toolTipText = value; }
        UI_API void setDragData(std::string value) { _dragData = value; }
        UI_API void setDragTexture(texture* value) { _dragTexture = value; }

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