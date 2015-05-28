#include "control.h"

namespace phi
{
    control::control(size<GLuint> viewportSize)
    {
        _x = 0;
        _y = 0;
        _zIndex = 0;
        _size = size<GLuint>(0, 0);
        _viewportSize = size<GLuint>();

        _isFocused = false;
        _isMouseOver = false;
        _mouseEnter = new mouseEventHandler();
        _mouseLeave = new mouseEventHandler();
        _gotFocus = new eventHandler<controlEventArgs>();
        _lostFocus = new eventHandler<controlEventArgs>();
    }

    control::~control()
    {
        DELETE(_mouseEnter);
        DELETE(_mouseLeave);
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
    }

    void control::notifyMouseUp(mouseEventArgs* e)
    {
        onMouseUp(e);
    }

    void control::notifyMouseMove(mouseEventArgs* e)
    {
        onMouseMove(e);
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
}