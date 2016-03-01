#ifndef _PHI_FLOAT_ANIMATION_H_
#define _PHI_FLOAT_ANIMATION_H_

#include "easingFunctions.h"

#include <functional>

namespace phi
{
    class floatAnimation
    {
    private:
        float* _value;
        float _from;
        float _to;
        int _milliseconds;
        int _elapsed;
        std::function<void(float)> _callback;
        int _delayMilliseconds;
        int _delayElapsed;
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
        int getMilliseconds() { return _milliseconds; }
        int getElapsed() { return _elapsed; }
        std::function<void(float)> getCallback() { return _callback; }
        int getDelayMilliseconds() { return _delayMilliseconds; }
        int getDelayElapsed() { return _delayElapsed; }
        std::function<float(float)> getEasingFunction() { return _easingFunction; }

        void setElapsed(int value) { _elapsed = value; }
        void setDelayElapsed(int value) { _delayElapsed = value; }
    };
}

#endif