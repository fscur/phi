#ifndef _PHI_COLOR_ANIMATOR_H_
#define _PHI_COLOR_ANIMATOR_H_

#include "phi/core/globals.h"
#include "phi/core/color.h"
#include "phi/core/mathUtils.h"

#include "phi/ui/colorAnimation.h"
#include "phi/ui/ui.h"

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