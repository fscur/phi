#ifndef _PHI_COLOR_ANIMATION_H_
#define _PHI_COLOR_ANIMATION_H_

#include <core\color.h>

#include "easingFunctions.h"

#include <functional>

namespace phi
{
    class colorAnimation
    {
    private:
        color* _colorFromVar;
        color _colorFrom;
        color _colorTo;
        int _milliseconds;
        int _elapsed;
        std::function<float(float)> _easingFunction;

    public:
        colorAnimation(color* from, color to, int milliseconds, std::function<float(float)> easingFunction = easingFunctions::linear)
        {
            _colorFromVar = from;
            _colorFrom = *from;
            _colorTo = to;
            _milliseconds = milliseconds;
            _elapsed = 0;
            _easingFunction = easingFunction;
        }

        color* getColorFromVar() { return _colorFromVar; }
        color getColorFrom() { return _colorFrom; }
        color getColorTo() { return _colorTo; }
        int getMilliseconds() { return _milliseconds; }
        int getElapsed() { return _elapsed; }
        std::function<float(float)> getEasingFunction() { return _easingFunction; }

        void setElapsed(int value) { _elapsed = value; }
    };
}

#endif