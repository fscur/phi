#ifndef _PHI_EASING_FUNCTIONS_H_
#define _PHI_EASING_FUNCTIONS_H_

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
    };
}

#endif