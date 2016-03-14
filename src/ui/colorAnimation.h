#pragma once
#include <phi.h>

#include <core\color.h>

#include "easingFunctions.h"

namespace phi
{
    class colorAnimation
    {
    private:
        color* _colorFromVar;
        color _colorFrom;
        color _colorTo;
        double _milliseconds;
        double _elapsed;
        std::function<float(float)> _easingFunction;

    public:
        colorAnimation(color* from, color to, double milliseconds, std::function<float(float)> easingFunction = easingFunctions::linear)
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
        double getMilliseconds() { return _milliseconds; }
        double getElapsed() { return _elapsed; }
        std::function<float(float)> getEasingFunction() { return _easingFunction; }

        void setElapsed(double value) { _elapsed = value; }
    };
}