#pragma once
#include <phi.h>

#include "easingFunctions.h"

namespace phi
{
    class floatAnimation
    {
    private:
        float* _value;
        float _from;
        float _to;
        double _milliseconds;
        double _elapsed;
        std::function<void(float)> _callback;
        double _delayMilliseconds;
        double _delayElapsed;
        std::function<float(float)> _easingFunction;

    public:
        floatAnimation(float* value, float to, int milliseconds, std::function<void(float)> callback = nullptr, int delayMilliseconds = 0, std::function<float(float)> easingFunction = easingFunctions::linear)
        {
            _value = value;
            _from = *value;
            _to = to;
            _milliseconds = milliseconds;
            _elapsed = 0;
            _callback = callback;
            _delayMilliseconds = delayMilliseconds;
            _delayElapsed = 0;
            _easingFunction = easingFunction;
        }

        float* getValue() { return _value; }
        float getFrom() { return _from; }
        float getTo() { return _to; }
        double getMilliseconds() { return _milliseconds; }
        double getElapsed() { return _elapsed; }
        std::function<void(float)> getCallback() { return _callback; }
        double getDelayMilliseconds() { return _delayMilliseconds; }
        double getDelayElapsed() { return _delayElapsed; }
        std::function<float(float)> getEasingFunction() { return _easingFunction; }

        void setElapsed(double value) { _elapsed = value; }
        void setDelayElapsed(double value) { _delayElapsed = value; }
    };
}