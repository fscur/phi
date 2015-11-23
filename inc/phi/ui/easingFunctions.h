#ifndef _PHI_EASING_FUNCTIONS_H_
#define _PHI_EASING_FUNCTIONS_H_

#include <cmath>

namespace phi
{
    class easingFunctions
    {
    public:
        // no easing, no acceleration
        static float linear(float t) { return t; }

        // accelerating from zero velocity
        static float easeInQuad(float t) { return t*t; }

        // decelerating to zero velocity
        static float easeOutQuad(float t) { return t*(2 - t); }

        // acceleration until halfway, then deceleration
        static float easeInOutQuad(float t) { return t < .5 ? 2 * t*t : -1 + (4 - 2 * t)*t; }

        // accelerating from zero velocity
        static float easeInCubic(float t) { return t*t*t; }

        // decelerating to zero velocity
        static float easeOutCubic(float t) { return (--t)*t*t + 1; }

        // acceleration until halfway, then deceleration
        static float easeInOutCubic(float t) { return t < .5 ? 4 * t*t*t : (t - 1)*(2 * t - 2)*(2 * t - 2) + 1; }

        // accelerating from zero velocity 
        static float easeInQuart(float t) { return t*t*t*t; }

        // decelerating to zero velocity 
        static float easeOutQuart(float t) { return 1 - (--t)*t*t*t; }

        // acceleration until halfway, then deceleration
        static float easeInOutQuart(float t) { return t < .5 ? 8 * t*t*t*t : 1 - 8 * (--t)*t*t*t; }

        // accelerating from zero velocity
        static float easeInQuint(float t) { return t*t*t*t*t; }

        // decelerating to zero velocity
        static float easeOutQuint(float t) { return 1 + (--t)*t*t*t*t; }

        // acceleration until halfway, then deceleration 
        static float easeInOutQuint(float t) { return t < .5 ? 16 * t*t*t*t*t : 1 + 16 * (--t)*t*t*t*t; }

        static float easeOutBounce(float t)
        {
            if (t < (1 / 2.75))
                return 7.5625*t*t;
            else if (t < (2 / 2.75))
                return 7.5625*(t -= (1.5 / 2.75))*t + .75;
            else if (t < (2.5 / 2.75))
                return 7.5625*(t -= (2.25 / 2.75))*t + .9375;
            else
                return 7.5625*(t -= (2.625 / 2.75))*t + .984375;
        }

        static float easeOutElastic(float t)
        {
            float s = 1.70158;
            float p = 0;
            float a = 1;
            if (t == 0)
                return 0;
            if (t == 1)
                return 1;
            if (!p)
                p = .3;
            if (a < 1)
            {
                a = 1;
                float s = p / 4;
            }
            else
                s = p / (2 * 3.14159265358979323846) * std::asin(1 / a);
            return a*std::pow(2, -10 * t) * std::sin((t - s)*(2 * 3.14159265358979323846) / p) + 1;
        }
    };
}

#endif