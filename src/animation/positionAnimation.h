#pragma once
#include <phi.h>
#include "animationApi.h"
#include "animation.h"
#include "easingFunctions.h"

#include <core\transform.h>

namespace phi
{
    class positionAnimation :
        public animation
    {
    private:
        transform* _value;
        vec3 _from;
        vec3 _to;

    public:
        ANIMATION_API positionAnimation(
            transform* value,
            std::function<double(double)> easingFunction = easingFunctions::linear);

        ANIMATION_API positionAnimation(const positionAnimation& original);
        ANIMATION_API ~positionAnimation();
        ANIMATION_API void start(vec3 from, vec3 to, double duration);
        ANIMATION_API void update(double t) override;
        ANIMATION_API animation* clone() override;
    };
}