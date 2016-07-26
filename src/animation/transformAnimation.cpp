#include <precompiled.h>
#include "transformAnimation.h"

namespace phi
{
    transformAnimation::transformAnimation(
        transform* value, 
        std::function<double(double)> easingFunction) :
        animation<transform>(value, easingFunction)
    {
    }

    transformAnimation::transformAnimation(const transformAnimation & original) :
        animation<transform>(original)
    {
    }

    transformAnimation::~transformAnimation()
    {
    }

    void transformAnimation::update(double t)
    {
        auto fromPosition = _from->getLocalPosition();
        auto toPosition = _to->getLocalPosition(); 
        auto position = fromPosition + (toPosition - fromPosition) * static_cast<float>(t);
        _value->setLocalPosition(position);
    }

    iAnimation* transformAnimation::clone()
    {
        auto original = static_cast<const transformAnimation*>(this);
        return new transformAnimation(*original);
    }
}