#pragma once
#include <phi.h>

#include "ui.h"
#include "floatAnimation.h"

namespace phi
{
    class floatAnimator
    {
    private:
        static std::vector<floatAnimation*> _animations;
        static std::vector<floatAnimation*> _toCancelAnimations;
        static double _lastUpdateMilliseconds;

    public:
        UI_API static void animateFloat(float* floatFrom, float floatTo, int milliseconds);
        UI_API static void animateFloat(floatAnimation* animation);
        UI_API static void cancelAnimation(floatAnimation* animation);
        UI_API static void update();
    };
}