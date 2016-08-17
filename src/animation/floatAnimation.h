#pragma once
#include <phi.h>
#include "animationApi.h"
#include "easingFunctions.h"
#include "animation.h"

namespace phi
{
    class floatAnimation :
        public animation
    {
    private:
        float _from;
        float _to;
        std::function<void(float)> _updateFunction;

    public:
        ANIMATION_API floatAnimation(
            std::function<void(float)> updateFunction,
            std::function<double(double)> easingFunction = easingFunctions::linear);
        ANIMATION_API floatAnimation(const floatAnimation& original);
        ANIMATION_API ~floatAnimation();

        ANIMATION_API void start(float from, float to, double duration);
        ANIMATION_API void update(double t) override;
        ANIMATION_API animation* clone() override;
    };
}