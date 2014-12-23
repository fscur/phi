#ifndef _PHI_COLOR_ANIMATION_H_
#define _PHI_COLOR_ANIMATION_H_

#include "color.h"

namespace phi
{
    class colorAnimation
    {
    private:
        color* _colorFrom;
        color _colorTo;

    public:
        colorAnimation(color* from, color to)
        {
            _colorFrom = from;
            _colorTo = to;
        }

        color* getColorFrom() { return _colorFrom; }
        color getColorTo() { return _colorTo; }
    };
}

#endif