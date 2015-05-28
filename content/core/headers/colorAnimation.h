#ifndef _PHI_COLOR_ANIMATION_H_
#define _PHI_COLOR_ANIMATION_H_

#include "color.h"

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

    public:
        colorAnimation(color* from, color to, int milliseconds)
        {
            _colorFromVar = from;
            _colorFrom = *from;
            _colorTo = to;
            _milliseconds = milliseconds;
            _elapsed = 0;
        }

        color* getColorFromVar() { return _colorFromVar; }
        color getColorFrom() { return _colorFrom; }
        color getColorTo() { return _colorTo; }
        int getMilliseconds() { return _milliseconds; }
        int getElapsed() { return _elapsed; }

        void setElapsed(int value) { _elapsed = value; }
    };
}

#endif