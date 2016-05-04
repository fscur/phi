#pragma once
#include <phi.h>

#include "animationApi.h"
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
        ANIMATION_API static void animateFloat(floatAnimation* animation);
        ANIMATION_API static void cancelAnimation(floatAnimation* animation);
        ANIMATION_API static void update();
    };
}