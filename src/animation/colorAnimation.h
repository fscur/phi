#pragma once
#include <phi.h>
#include "animationApi.h"

#include <core/color.h>

#include "easingFunctions.h"
#include "animation.h"

namespace phi
{
    class colorAnimation :
        public animation
    {
    private:
        color _from;
        color _to;
        std::function<void(color)> _updateFunction;

    public:
        ANIMATION_API colorAnimation();
        ANIMATION_API colorAnimation(std::function<void(color)> callback);
        ANIMATION_API colorAnimation(const colorAnimation& original);
        ANIMATION_API ~colorAnimation();

        ANIMATION_API void setUpdateFunction(std::function<void(color)> callback) { _updateFunction = callback; }

        ANIMATION_API void start(color from, color to, double duration, std::function<void(void)> animationEndedCallback);
        ANIMATION_API void start(color from, color to, double duration);

        ANIMATION_API void update(double t) override;
        ANIMATION_API animation* clone() override;
    };
}