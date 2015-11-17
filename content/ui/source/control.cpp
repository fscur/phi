#include "control.h"
#include "clock.h"
#include <algorithm>

namespace phi
{
    toolTip* control::_toolTip = nullptr;
    scissorStack* control::controlsScissors = nullptr;

    control::control(size<GLuint> viewportSize)
    {
        _x = 0;
        _y = 0;
        _zIndex = 0;
        _size = size<GLuint>(0, 0);
        _viewportSize = viewportSize;
        _isFocused = false;
        _isMouseOver = false;
        _isTopMost = false;
        _toolTipText = "";
        _renderToolTip = false;
        _dragData = "";
        _clickedOver = false;
        _mouseEnter = new mouseEventHandler();
        _mouseLeave = new mouseEventHandler();
        _gotFocus = new eventHandler<controlEventArgs>();
        _lostFocus = new eventHandler<controlEventArgs>();
        _addedChild = new eventHandler<controlEventArgs>();
        _removedChild = new eventHandler<controlEventArgs>();
    }

    control::~control()
    {
        DELETE(_mouseEnter);
        DELETE(_mouseLeave);
    }

    void control::init(size<GLuint> viewportSize)
    {
        _toolTip = new toolTip(viewportSize);
        controlsScissors = new scissorStack(viewportSize);
    }

    void control::addChild(control* child)
    {
        _children.push_back(child);

        if (_addedChild->isBound())
            _addedChild->invoke(controlEventArgs(child));
    }

    void control::removeChild(control* child)
    {
        _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());

        if (_removedChild->isBound())
            _removedChild->invoke(controlEventArgs(child));
    }

    void control::setIsFocused(bool value)
    {
        bool gotFocus = !_isFocused && value;
        bool lostFocus = _isFocused && !value;
        _isFocused = value;
        if (gotFocus)
            notifyGotFocus(controlEventArgs(this));
        if (lostFocus)
            notifyLostFocus(controlEventArgs(this));
    }

    bool control::isPointInside(int x, int y)
    {
        return x >= _x && x <= _x + (int)_size.width && y >= _y && y <= _y + (int)_size.height;
    }

    void control::notifyMouseDown(mouseEventArgs* e)
    {
        onMouseDown(e);
        if (getIsMouseOver() && e->leftButtonPressed)
            _clickedOver = true;
    }

    void control::notifyMouseUp(mouseEventArgs* e)
    {
        onMouseUp(e);
        if (e->leftButtonPressed && _clickedOver)
            _clickedOver = false;
    }

    void control::notifyMouseMove(mouseEventArgs* e)
    {
        onMouseMove(e);
        resetToolTip();
        if (!_dragData.empty() && _clickedOver && !dragDropController::get()->getIsDragging())
            dragDropController::get()->startDrag(_dragData, _dragTexture);
    }

    void control::notifyMouseWheel(mouseEventArgs* e)
    {
        onMouseWheel(e);
    }

    void control::notifyKeyDown(keyboardEventArgs e)
    {
        onKeyDown(e);
    }

    void control::notifyKeyUp(keyboardEventArgs e)
    {
        onKeyUp(e);
    }

    void control::notifyMouseEnter(mouseEventArgs* e)
    {
        if (!_isMouseOver)
        {
            _isMouseOver = true;
            resetToolTip();

            onMouseEnter(e);
            if (_mouseEnter->isBound())
                _mouseEnter->invoke(e);
        }
    }

    void control::notifyMouseLeave(mouseEventArgs* e)
    {
        if (_isMouseOver)
        {
            _isMouseOver = false;
            resetToolTip();

            onMouseLeave(e);
            if (_mouseLeave->isBound())
                _mouseLeave->invoke(e);
        }
    }

    void control::notifyGotFocus(controlEventArgs e)
    {
        if (_gotFocus->isBound())
            _gotFocus->invoke(e);
    }

    void control::notifyLostFocus(controlEventArgs e)
    {
        if (_lostFocus->isBound())
            _lostFocus->invoke(e);
    }

    void control::resetToolTip()
    {
        _mouseStillTime = 0.0f;
        _renderToolTip = false;
    }

    void control::update()
    {
        if (_isMouseOver && !_renderToolTip)
        {
            _mouseStillTime += clock::millisecondsElapsed;

            if (_mouseStillTime > 1000.0f && _toolTipText != "")
            {
                //_toolTip->setText(_toolTipText);
                //auto toolTipSize = _toolTip->getSize();
                //auto x = glm::max(0.0f, _x + _size.width * 0.5f - toolTipSize.width * 0.5f);
                //x = glm::min(x, (float)(_viewportSize.width - toolTipSize.width));
                //float y;
                //if (_y + _size.height + toolTipSize.height > _viewportSize.height)
                //    y = _y - toolTipSize.height;
                //else
                //    y = _y + _size.height;

                _toolTip->show(_toolTipText, glm::vec2(_x + _size.width * 0.5f, _y + _size.height), _size);

                _renderToolTip = true;
            }
        }
    }

    void control::render()
    {
        if (_renderToolTip)
            _toolTip->render();

        onRender();
    }

    void control::onRender()
    {
        for (control* ctrl : _children)
            ctrl->render();
    }
}