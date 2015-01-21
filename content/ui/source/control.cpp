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

        input::mouseDown->bind<control, &control::inputMouseDown>(this);
        input::mouseUp->bind<control, &control::inputMouseUp>(this);
        input::mouseMove->bind<control, &control::inputMouseMove>(this);
        input::mouseWheel->bind<control, &control::inputMouseWheel>(this);
        input::keyDown->bind<control, &control::inputKeyDown>(this);
        input::keyDown->bind<control, &control::inputKeyUp>(this);
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

    void control::notifyMouseEnter(mouseEventArgs e)
    {
        if (_mouseEnter->isBound())
            _mouseEnter->invoke(e);
    }

    void control::notifyMouseLeave(mouseEventArgs e)
    {
        if (_mouseLeave->isBound())
            _mouseLeave->invoke(e);
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

    void control::inputMouseDown(mouseEventArgs e)
    {
        if (_isMouseOver)
            setIsFocused(true);

        onMouseDown(e);
    }

    void control::inputMouseUp(mouseEventArgs e)
    {
        onMouseUp(e);
    }

    void control::inputMouseMove(mouseEventArgs e)
    {
        if (e.x >= _x && e.x <= _x + (int)_size.width && e.y >= _y && e.y <= _y + (int)_size.height)
        {
            if (!_isMouseOver)
            {
                _isMouseOver = true;

                onMouseEnter(e);
                notifyMouseEnter(e);
            }
        }
        else
        {
            if (_isMouseOver)
            {
                _isMouseOver = false;

                onMouseLeave(e);
                notifyMouseLeave(e);
            }
        }

        onMouseMove(e);
    }

    void control::inputMouseWheel(mouseEventArgs e)
    {
        onMouseWheel(e);
    }

    void control::inputKeyDown(keyboardEventArgs e)
    {
        if (getIsFocused())
            onKeyDown(e);
    }

    void control::inputKeyUp(keyboardEventArgs e)
    {
        if (getIsFocused())
            onKeyUp(e);
    }
}