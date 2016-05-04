#pragma once
#include <phi.h>

#include "easingFunctions.h"

namespace phi
{
    class floatAnimation
    {
    private:
        float _from;
        float _to;
        double _milliseconds;
        double _elapsed;
        std::function<void(float)> _callback;
        double _delayMilliseconds;
        double _delayElapsed;
        std::function<float(float)> _easingFunction;
        std::function<void()> _endCallback;

    public:
        floatAnimation(float from, float to, int milliseconds, std::function<void(float)> callback, int delayMilliseconds = 0, std::function<float(float)> easingFunction = easingFunctions::linear, std::function<void()> endCallback = nullptr) :
            _from(from),
            _to(to),
            _milliseconds(milliseconds),
            _elapsed(0),
            _callback(callback),
            _delayMilliseconds(delayMilliseconds),
            _delayElapsed(0),
            _easingFunction(easingFunction),
            _endCallback(endCallback)
        {
        }

        float getFrom() { return _from; }
        float getTo() { return _to; }
        double getMilliseconds() { return _milliseconds; }
        double getElapsed() { return _elapsed; }
        std::function<void(float)> getCallback() { return _callback; }
        double getDelayMilliseconds() { return _delayMilliseconds; }
        double getDelayElapsed() { return _delayElapsed; }
        std::function<float(float)> getEasingFunction() { return _easingFunction; }
        std::function<void()> getEndCallback() { return _endCallback; }

        void setElapsed(double value) { _elapsed = value; }
        void setDelayElapsed(double value) { _delayElapsed = value; }
    };
}