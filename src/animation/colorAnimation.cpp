#include <precompiled.h>
#include "colorAnimation.h"

namespace phi
{
    colorAnimation::colorAnimation() :
        animation(easingFunctions::linear)
    {
    }

    colorAnimation::colorAnimation(std::function<void(color)> callback) :
        animation(easingFunctions::linear),
        _updateFunction(callback)
    {
    }

    colorAnimation::colorAnimation(const colorAnimation & original) :
        animation(original)
    {
    }

    void colorAnimation::start(color from, color to, double duration, std::function<void(void)> animationEndedCallback)
    {
        animation::start(duration, animationEndedCallback);
        _from = from;
        _to = to;
    }

    void colorAnimation::start(color from, color to, double duration)
    {
        animation::start(duration);
        _from = from;
        _to = to;
    }

    colorAnimation::~colorAnimation()
    {
    }

    void colorAnimation::update(double t)
    {
        _updateFunction(_from + (_to - _from) * static_cast<float>(t));
    }

    animation* colorAnimation::clone()
    {
        auto original = static_cast<const colorAnimation*>(this);
        return new colorAnimation(*original);
    }
}