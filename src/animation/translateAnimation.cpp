#include <precompiled.h>
#include "translateAnimation.h"

namespace phi
{
    translateAnimation::translateAnimation(
        std::function<void(vec3)> updateCallback,
        std::function<double(double)> easingFunction) :
        animation(easingFunction),
        _updateCallback(updateCallback)
    {
    }

    translateAnimation::translateAnimation(const translateAnimation & original) :
        animation(original)
    {
    }

    translateAnimation::~translateAnimation()
    {
    }

    void translateAnimation::start(vec3 from, vec3 to, double duration)
    {
        animation::start(duration);
        _from = from;
        _to = to;
    }

    void translateAnimation::update(double t)
    {
        auto factor = static_cast<float>(t);
        auto updatedValue = _from + (_to - _from) * factor;

        _updateCallback(updatedValue);
    }

    animation* translateAnimation::clone()
    {
        auto original = static_cast<const translateAnimation*>(this);
        return new translateAnimation(*original);
    }
}