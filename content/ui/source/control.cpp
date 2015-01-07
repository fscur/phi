#include "control.h"

namespace phi
{
    control::control()
    {
        _isMouseOver = false;
        _mouseEnter = new mouseEventHandler();
        _mouseLeave = new mouseEventHandler();

        input::mouseDown->bind<control, &control::inputMouseDown>(this);
        input::mouseUp->bind<control, &control::inputMouseUp>(this);
        input::mouseMove->bind<control, &control::inputMouseMove>(this);
        input::mouseWheel->bind<control, &control::inputMouseWheel>(this);
    }

    control::~control()
    {
        DELETE(_mouseEnter);
        DELETE(_mouseLeave);
    }

    void control::inputMouseDown(mouseEventArgs e)
    {
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
                if (_mouseEnter->isBound())
                    _mouseEnter->invoke(e);
            }
        }
        else
        {
            if (_isMouseOver)
            {
                _isMouseOver = false;

                onMouseLeave(e);
                if (_mouseLeave->isBound())
                    _mouseLeave->invoke(e);
            }
        }

        onMouseMove(e);
    }

    void control::inputMouseWheel(mouseEventArgs e)
    {
        onMouseWheel(e);
    }
}