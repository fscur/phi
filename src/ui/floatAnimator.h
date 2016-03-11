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
        static double _lastUpdateMilliseconds;

    public:
        static void animateFloat(float* floatFrom, float floatTo, int milliseconds);
        static void animateFloat(floatAnimation* animation);
        static void update();
    };
}