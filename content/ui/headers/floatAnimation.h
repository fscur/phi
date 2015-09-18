#ifndef _PHI_FLOAT_ANIMATION_H_
#define _PHI_FLOAT_ANIMATION_H_

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

    public:
        floatAnimation(float* value, float to, int milliseconds, std::function<void(float)> callback = nullptr)
        {
            _value = value;
            _from = *value;
            _to = to;
            _milliseconds = milliseconds;
            _elapsed = 0;
            _callback = callback;
        }

        float* getValue() { return _value; }
        float getFrom() { return _from; }
        float getTo() { return _to; }
        int getMilliseconds() { return _milliseconds; }
        int getElapsed() { return _elapsed; }
        std::function<void(float)> getCallback() { return _callback; }

        void setElapsed(int value) { _elapsed = value; }
    };
}

#endif