#pragma once
#include <phi.h>
#include "animationApi.h"
#include "animation.h"
#include "easingFunctions.h"

namespace phi
{
    class translateAnimation :
        public animation
    {
    private:
        std::function<void(vec3)> _updateCallback;
        vec3 _from;
        vec3 _to;

    public:
        ANIMATION_API translateAnimation(
            std::function<void(vec3)> updateCallback,
            std::function<double(double)> easingFunction = easingFunctions::linear);

        ANIMATION_API translateAnimation(const translateAnimation& original);
        ANIMATION_API ~translateAnimation();
        ANIMATION_API void start(vec3 from, vec3 to, double duration);
        ANIMATION_API void update(double t) override;
        ANIMATION_API animation* clone() override;
    };
}