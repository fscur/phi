#pragma once
#include <phi.h>

#include <core\input.h>
#include <core\size.h>

#include "uiApi.h"
#include "uiRepository.h"
#include "controlEventArgs.h"
#include "scissorStack.h"
#include "dragDropController.h"

namespace phi
{
    namespace controlType
    {
        enum controlType
        {
            panel,
            label,
            button,
            slider,
            image
        };
    }

    class control : 
        public eventSender
    {
    protected:
        static scissorStack* controlsScissors;

    private:
        mouseEventHandler* _mouseEnter;
        mouseEventHandler* _mouseLeave;
        eventHandler<control*>* _propertyChanged;
        eventHandler<controlEventArgs>* _gotFocus;
        eventHandler<controlEventArgs>* _lostFocus;
        eventHandler<controlEventArgs>* _addedChild;
        eventHandler<controlEventArgs>* _removedChild;
        texture* _dragTexture;
        vector<control*> _children;
        string _dragData;
        string _toolTipText;
        double _mouseStillTime;
        bool _isFocused;
        bool _isTopMost;
        bool _isMouseOver;
        bool _clickedOver;
        bool _renderToolTip;

    protected:
        vec3 _position;
        float _zIndex;
        sizef _size;
        sizef _viewportSize;
        color _backgroundColor;
        texture* _backgroundTexture;
        vec4 _padding;

    public:
        controlType::controlType type;

    private:
        void resetToolTip();
        void notifyGotFocus(controlEventArgs e);
        void notifyLostFocus(controlEventArgs e);

    protected:
        void raisePropertyChanged();

    public:
        control(controlType::controlType type);
        virtual ~control();

        void addChild(control* child);
        void removeChild(control* child);

        mouseEventHandler* getMouseEnter() const { return _mouseEnter; }
        mouseEventHandler* getMouseLeave() const { return _mouseLeave; }

        eventHandler<control*>* getPropertyChanged() const { return _propertyChanged; }
        eventHandler<controlEventArgs>* getGotFocus() const { return _gotFocus; }
        eventHandler<controlEventArgs>* getLostFocus() const { return _lostFocus; }
        eventHandler<controlEventArgs>* getAddedChild() const { return _addedChild; }
        eventHandler<controlEventArgs>* getRemovedChild() const { return _removedChild; }

        virtual vec3 getPosition() { return _position; }
        virtual float getZIndex() { return _zIndex; }
        virtual sizef getSize() { return _size; }
        virtual bool getIsMouseOver() { return _isMouseOver; }
        virtual bool getIsFocused() { return _isFocused; }
        virtual bool getIsTopMost() { return _isTopMost; }
        string getToolTipText() const { return _toolTipText; }
        vector<control*> getChildren() { return _children; }
        string getDragData() { return _dragData; }
        texture* getDragTexture() { return _dragTexture; }

        color getBackgroundColor() const { return _backgroundColor; }
        texture* getBackgroundTexture() const { return _backgroundTexture; }
        vec4 getPadding() const { return _padding; }

        virtual void setPosition(vec3 value) { _position = value; resetToolTip(); }
        virtual void setZIndex(float value) { _zIndex = value; }
        virtual void setSize(sizef value) { _size = value; }
        virtual void setIsFocused(bool value);
        virtual void setIsTopMost(bool value) { _isTopMost = value; }
        void setToolTipText(const string value) { _toolTipText = value; }
        void setDragData(std::string value) { _dragData = value; }
        void setDragTexture(texture* value) { _dragTexture = value; }

        UI_API void setBackgroundColor(color value);
        UI_API void setBackgroundTexture(texture* value);
        UI_API void setPadding(vec4 value);

        virtual bool isPointInside(int x, int y);
        virtual void update();

        void virtual onMouseDown(mouseEventArgs* e) {}
        void virtual onMouseUp(mouseEventArgs* e) {}
        void virtual onMouseMove(mouseEventArgs* e) {}
        void virtual onMouseWheel(mouseEventArgs* e) {}
        void virtual onMouseEnter(mouseEventArgs* e) {}
        void virtual onMouseLeave(mouseEventArgs* e) {}
        void virtual onKeyDown(keyboardEventArgs* e) {}
        void virtual onKeyUp(keyboardEventArgs* e) {}

        void notifyMouseEnter(mouseEventArgs* e);
        void notifyMouseLeave(mouseEventArgs* e);
        void notifyMouseDown(mouseEventArgs* e);
        void notifyMouseUp(mouseEventArgs* e);
        void notifyMouseMove(mouseEventArgs* e);
        void notifyMouseWheel(mouseEventArgs* e);
        void notifyKeyDown(keyboardEventArgs* e);
        void notifyKeyUp(keyboardEventArgs* e);
    };
}