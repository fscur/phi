#include <precompiled.h>
#include "..\mouseButtonEventDispatcher.h"
#include <input\input.h>

namespace phi
{
    using namespace std::chrono;

    mouseButtonEventDispatcher::mouseButtonEventDispatcher(HWND windowHandle) :
        _windowHandle(windowHandle),
        _isLastButtonDownADoubleClick(false),
        _buttonDownLocation(ivec2()),
        _buttonDownFunction({}),
        _buttonClickFunction({}),
        _buttonDoubleClickFunction({}),
        _buttonUpFunction({})
    {
    }
    
    mouseButtonEventDispatcher::~mouseButtonEventDispatcher()
    {
    }

    void mouseButtonEventDispatcher::notifyButtonDown(int x, int y)
    {
        _buttonDownLocation = ivec2(x, y);
        _buttonDownFunction(_buttonDownLocation.x, _buttonDownLocation.y);
    }

    void mouseButtonEventDispatcher::notifyButtonDoubleClick(int x, int y)
    {
        _isLastButtonDownADoubleClick = true;
        notifyButtonDown(x, y);
    }

    void mouseButtonEventDispatcher::notifyButtonUp(int x, int y)
    {
        auto location = ivec2(x, y);

        if (_buttonDownLocation == location)
        {
            if (_isLastButtonDownADoubleClick)
            {
                _buttonDoubleClickFunction(x, y);
                _isLastButtonDownADoubleClick = false;
            }
            else
                _buttonClickFunction(x, y);
        }

        _buttonUpFunction(x, y);
    }
}