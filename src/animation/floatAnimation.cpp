#include <precompiled.h>
#include "floatAnimation.h"

namespace phi
{
    floatAnimation::floatAnimation(float* value, std::function<double(double)> easingFunction) :
        animation(easingFunction)
    {
    }

    floatAnimation::floatAnimation(const floatAnimation & original) :
        animation(original)
    {
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
        *_value = static_cast<float>(_from + (_to - _from) * t);
    }

    animation* floatAnimation::clone()
    {
        auto original = static_cast<const floatAnimation*>(this);
        return new floatAnimation(*original);
    }
}