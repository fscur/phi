#include <precompiled.h>
#include "floatAnimation.h"

namespace phi
{
    floatAnimation::floatAnimation() :
        animation(easingFunctions::linear)
    {
    }

    floatAnimation::floatAnimation(
        std::function<double(double)> easingFunction) :
        animation(easingFunction)
    {
    }

    /*floatAnimation::floatAnimation(
        std::function<void(float)> updateFunction,
        std::function<double(double)> easingFunction) :
        animation(easingFunction),
        _updateFunction(updateFunction)
    {
    }*/

    floatAnimation::floatAnimation(const floatAnimation & original) :
        animation(original)
    {
    }

    void floatAnimation::start(float from, float to, double duration, std::function<void(void)> animationEndedCallback)
    {
        animation::start(duration, animationEndedCallback);
        _from = from;
        _to = to;
    }

    void floatAnimation::start(float from, float to, double duration)
    {
        animation::start(duration);
        _from = from;
        _to = to;
    }

    floatAnimation::~floatAnimation()
    {
    }

    void floatAnimation::update(double t)
    {
        _updateFunction(static_cast<float>(_from + (_to - _from) * t));
    }

    animation* floatAnimation::clone()
    {
        auto original = static_cast<const floatAnimation*>(this);
        return new floatAnimation(*original);
    }
}