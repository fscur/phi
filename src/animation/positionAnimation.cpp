#include <precompiled.h>
#include "positionAnimation.h"

namespace phi
{
    positionAnimation::positionAnimation(
        transform* value,
        std::function<double(double)> easingFunction) :
        animation(easingFunction),
        _value(value)
    {
    }

    positionAnimation::positionAnimation(const positionAnimation & original) :
        animation(original)
    {
    }

    positionAnimation::~positionAnimation()
    {
    }

    void positionAnimation::start(vec3 from, vec3 to, double duration)
    {
        animation::start(duration);
        _from = from;
        _to = to;
    }

    void positionAnimation::update(double t)
    {
        auto position = _from + (_to - _from) * static_cast<float>(t);
        _value->setLocalPosition(position);
    }

    animation* positionAnimation::clone()
    {
        auto original = static_cast<const positionAnimation*>(this);
        return new positionAnimation(*original);
    }
}