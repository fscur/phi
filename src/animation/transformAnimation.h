#pragma once
#include <phi.h>
#include "animationApi.h"
#include "animation.h"
#include "easingFunctions.h"

#include <core\transform.h>

namespace phi
{
    class transformAnimation :
        public animation
    {
    private:
        transform* _value;
        transform* _from;
        transform* _to;

    public:
        ANIMATION_API transformAnimation(
            transform* value,
            std::function<double(double)> easingFunction = easingFunctions::linear);

        ANIMATION_API transformAnimation(const transformAnimation& original);
        ANIMATION_API ~transformAnimation();
        ANIMATION_API void start(transform* from, transform* to, double duration);
        ANIMATION_API void update(double t) override;
        ANIMATION_API animation* clone() override;
    };
}