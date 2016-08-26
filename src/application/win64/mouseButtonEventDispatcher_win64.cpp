#include <precompiled.h>
#include "..\mouseButtonEventDispatcher.h"
#include <core\input.h>

namespace phi
{
    using namespace std::chrono;

    mouseButtonEventDispatcher::mouseButtonEventDispatcher(HWND windowHandle) :
        _windowHandle(windowHandle),
        _isWaitingClickDuration(false),
        _isLastButtonDownADoubleClick(false),
        _shouldNotifyMouseUpOnUpdate(false),
        _buttonDownLocation(ivec2()),
        _buttonUpLocation(ivec2()),
        _clickEventDuration(static_cast<nanoseconds>(1000000 * CLICK_DURATION_IN_MILLISECONDS)),
        _buttonDownFunction({}),
        _buttonClickFunction({}),
        _buttonDoubleClickFunction({}),
        _buttonUpFunction({})
    {
        _lastButtonDownNotification = high_resolution_clock::now().time_since_epoch();
    }
    
    mouseButtonEventDispatcher::~mouseButtonEventDispatcher()
    {
    }

    void mouseButtonEventDispatcher::notifyButtonDown(int x, int y)
    {
        _lastButtonDownNotification = high_resolution_clock::now().time_since_epoch();
        _isWaitingClickDuration = true;
        _buttonDownLocation = ivec2(x, y);
    }

    void mouseButtonEventDispatcher::notifyButtonDoubleClick(int x, int y)
    {
        _isLastButtonDownADoubleClick = true;

        notifyButtonDown(x, y);
    }

    void mouseButtonEventDispatcher::notifyButtonUp(int x, int y)
    {
        auto now = high_resolution_clock::now().time_since_epoch();

        if (now - _lastButtonDownNotification < _clickEventDuration && !_isLastButtonDownADoubleClick)
        {
            _buttonClickFunction(x, y);
        }
        else if (_isLastButtonDownADoubleClick)
        {
            _buttonDoubleClickFunction(x, y);
            _isLastButtonDownADoubleClick = false;
        }
        else if (_isWaitingClickDuration)
        {
            _shouldNotifyMouseUpOnUpdate = true;
            _buttonUpLocation = ivec2(x, y);
        }
        else if(!_shouldNotifyMouseUpOnUpdate)
        {
            _buttonUpFunction(x, y);
        }
        
        _isWaitingClickDuration = false;
    }

    void mouseButtonEventDispatcher::update()
    {
        if (_isWaitingClickDuration)
        {
            auto now = high_resolution_clock::now().time_since_epoch();
            auto timeElapsedSinceLastButtonDownNotification = now - _lastButtonDownNotification;

            if (timeElapsedSinceLastButtonDownNotification > _clickEventDuration)
            {
                _buttonDownFunction(_buttonDownLocation.x, _buttonDownLocation.y);

                _isWaitingClickDuration = false;
                _isLastButtonDownADoubleClick = false;

                if (_shouldNotifyMouseUpOnUpdate)
                {
                    _buttonUpFunction(_buttonUpLocation.x, _buttonUpLocation.y);
                    _shouldNotifyMouseUpOnUpdate = false;
                }
            }
        }
    }
}