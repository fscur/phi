#include <precompiled.h>
#include "floatAnimation.h"

namespace phi
{
    floatAnimation::floatAnimation(float* value, std::function<double(double)> easingFunction) :
        animation<float>(value, easingFunction)
    {
    }

    floatAnimation::floatAnimation(const floatAnimation & original) :
        animation<float>(original)
    {
    }

    floatAnimation::~floatAnimation()
    {
    }

    void floatAnimation::update(double t)
    {
        *_value = static_cast<float>(*_from + (*_to - *_from) * t);
    }

    iAnimation* floatAnimation::clone()
    {
        auto original = static_cast<const floatAnimation*>(this);
        return new floatAnimation(*original);
    }
}