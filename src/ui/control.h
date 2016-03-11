#pragma once
#include <phi.h>

#include <core\input.h>
#include <core\size.h>

#include "ui.h"
#include "uiRepository.h"
#include "controlEventArgs.h"
#include "toolTip.h"
#include "scissorStack.h"
#include "dragDropController.h"

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
        double _mouseStillTime;
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
        sizeui _size;
        sizeui _viewportSize;

    public:
        control(sizeui viewportSize);
        virtual ~control();

        static void init(sizeui viewportSize);

        void addChild(control* child);
        void removeChild(control* child);

        mouseEventHandler* getMouseEnter() const { return _mouseEnter; }
        mouseEventHandler* getMouseLeave() const { return _mouseLeave; }
        eventHandler<controlEventArgs>* getGotFocus() const { return _gotFocus; }
        eventHandler<controlEventArgs>* getLostFocus() const { return _lostFocus; }
        eventHandler<controlEventArgs>* getAddedChild() const { return _addedChild; }
        eventHandler<controlEventArgs>* getRemovedChild() const { return _removedChild; }

        virtual int getX() { return _x; }
        virtual int getY() { return _y; }
        virtual float getZIndex() { return _zIndex; }
        virtual sizeui getSize() { return _size; }
        virtual bool getIsMouseOver() { return _isMouseOver; }
        virtual bool getIsFocused() { return _isFocused; }
        virtual bool getIsTopMost() { return _isTopMost; }
        virtual sizeui getViewportSize() { return _viewportSize; }
        std::string getToolTipText() const { return _toolTipText; }
        std::vector<control*> getChildren() { return _children; }
        std::string getDragData() { return _dragData; }
        texture* getDragTexture() { return _dragTexture; }

        virtual void setX(int value) { _x = value; resetToolTip(); }
        virtual void setY(int value) { _y = value; resetToolTip(); }
        virtual void setZIndex(float value) { _zIndex = value; }
        virtual void setSize(sizeui value) { _size = value; }
        virtual void setViewportSize(sizeui value) { _viewportSize = value; }
        virtual void setIsFocused(bool value);
        virtual void setIsTopMost(bool value) { _isTopMost = value; }
        void setToolTipText(const std::string value) { _toolTipText = value; }
        void setDragData(std::string value) { _dragData = value; }
        void setDragTexture(texture* value) { _dragTexture = value; }

        virtual bool isPointInside(int x, int y);
        virtual void update();
        void render();

        void virtual onRender();
        void virtual onMouseDown(mouseEventArgs* e) {}
        void virtual onMouseUp(mouseEventArgs* e) {}
        void virtual onMouseMove(mouseEventArgs* e) {}
        void virtual onMouseWheel(mouseEventArgs* e) {}
        void virtual onMouseEnter(mouseEventArgs* e) {}
        void virtual onMouseLeave(mouseEventArgs* e) {}
        void virtual onKeyDown(keyboardEventArgs e) {}
        void virtual onKeyUp(keyboardEventArgs e) {}

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