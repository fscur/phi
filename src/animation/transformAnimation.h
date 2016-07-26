#pragma once
#include <phi.h>
#include "animationApi.h"
#include "animation.h"
#include "easingFunctions.h"

#include <core\transform.h>

namespace phi
{
    class transformAnimation :
        public animation<transform>
    {
    public:
        ANIMATION_API transformAnimation(
            transform* value,
            std::function<double(double)> easingFunction = easingFunctions::linear);
        ANIMATION_API transformAnimation(const transformAnimation & original);
        ANIMATION_API ~transformAnimation();

        ANIMATION_API void update(double t) override;
        ANIMATION_API iAnimation* clone() override;
    };
}