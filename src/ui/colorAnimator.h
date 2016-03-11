#pragma once
#include <phi.h>

#include <core\color.h>
#include <core\mathUtils.h>

#include "colorAnimation.h"
#include "ui.h"

namespace phi
{
    class colorAnimator
    {
    private:
        static std::unordered_map<color*, colorAnimation*> _animations;
        static int _lastUpdateMilliseconds;

    public:
        static void animateColor(color* colorFrom, color colorTo, int milliseconds);
        static void animateColor(colorAnimation* animation);
        static void update();
    };
}