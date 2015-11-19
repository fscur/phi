#ifndef _PHI_COLOR_ANIMATOR_H_
#define _PHI_COLOR_ANIMATOR_H_

#include "ui.h"
#include "globals.h"
#include "color.h"
#include "mathUtils.h"
#include "colorAnimation.h"
#include <unordered_map>

namespace phi
{
    class colorAnimator
    {
    private:
        static std::unordered_map<color*, colorAnimation*> _animations;
        static int _lastUpdateMilliseconds;

    public:
        UI_API static void animateColor(color* colorFrom, color colorTo, int milliseconds);
        UI_API static void animateColor(colorAnimation* animation);
        UI_API static void update();
    };
}

#endif