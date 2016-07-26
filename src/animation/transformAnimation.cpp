#include <precompiled.h>
#include "transformAnimation.h"

namespace phi
{
    transformAnimation::transformAnimation(
        transform* value, 
        std::function<double(double)> easingFunction) :
        animation(easingFunction),
        _value(value)
    {
    }

    transformAnimation::transformAnimation(const transformAnimation & original) :
        animation(original)
    {
    }

    transformAnimation::~transformAnimation()
    {
    }

    void transformAnimation::start(transform* from, transform* to, double duration)
    {
        animation::start(duration);
        _from = from;
        _to = to;
    }

    void transformAnimation::update(double t)
    {
        auto fromPosition = _from->getLocalPosition();
        auto toPosition = _to->getLocalPosition(); 
        auto position = fromPosition + (toPosition - fromPosition) * static_cast<float>(t);
        _value->setLocalPosition(position);
    }

    animation* transformAnimation::clone()
    {
        auto original = static_cast<const transformAnimation*>(this);
        return new transformAnimation(*original);
    }
}