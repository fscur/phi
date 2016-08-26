#pragma once
#include <phi.h>

namespace phi
{
    class mouseButtonEventDispatcher
    {
    private:
        const int CLICK_DURATION_IN_MILLISECONDS = 200;

        HWND _windowHandle;

        bool _isWaitingClickDuration;
        bool _isLastButtonDownADoubleClick;
        bool _shouldNotifyMouseUpOnUpdate;

        ivec2 _buttonDownLocation;
        ivec2 _buttonUpLocation;

        std::chrono::nanoseconds _lastButtonDownNotification;
        std::chrono::nanoseconds _clickEventDuration;

        std::function<void(int x, int y)> _buttonDownFunction;
        std::function<void(int x, int y)> _buttonClickFunction;
        std::function<void(int x, int y)> _buttonDoubleClickFunction;
        std::function<void(int x, int y)> _buttonUpFunction;

    public:
        mouseButtonEventDispatcher(HWND windowHandle);
        ~mouseButtonEventDispatcher();

        void setButtonDownFunction(std::function<void(int x, int y)> value) { _buttonDownFunction = value; }
        void setButtonClickFunction(std::function<void(int x, int y)> value) { _buttonClickFunction = value; }
        void setButtonDoubleClickFunction(std::function<void(int x, int y)> value) { _buttonDoubleClickFunction = value; }
        void setButtonUpFunction(std::function<void(int x, int y)> value) { _buttonUpFunction = value; }

        void notifyButtonDown(int x, int y);
        void notifyButtonDoubleClick(int x, int y);
        void notifyButtonUp(int x, int y);
        void update();
    };
}