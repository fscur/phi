#pragma once
#include <phi.h>

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
        static float easeOutQuad(float t) { return t * (2.0f - t); }

        // acceleration until halfway, then deceleration
        static float easeInOutQuad(float t) 
        { 
            return t < 0.5f ? 
                2.0f * t*t : 
                -1.0f + (4.0f - 2.0f * t) * t; 
        }

        // accelerating from zero velocity
        static float easeInCubic(float t) { return t*t*t; }

        // decelerating to zero velocity
        static float easeOutCubic(float t) 
        { 
            --t;
            return t*t*t + 1.0f; 
        }

        // acceleration until halfway, then deceleration
        static float easeInOutCubic(float t) 
        { 
            return t < 0.5f ? 
                4.0f * t*t*t : 
                (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f; 
        }

        // accelerating from zero velocity 
        static float easeInQuart(float t) { return t*t*t*t; }

        // decelerating to zero velocity 
        static float easeOutQuart(float t) 
        {
            --t;
            return 1.0f - t*t*t*t; 
        }

        // acceleration until halfway, then deceleration
        static float easeInOutQuart(float t) 
        {
            if (t < 0.5f)
            {
                return 8.0f * t*t*t*t;
            }
            else
            {
                --t;
                return 1.0f - 8.0f * t*t*t*t;
            }
        }

        // accelerating from zero velocity
        static float easeInQuint(float t) { return t*t*t*t*t; }

        // decelerating to zero velocity
        static float easeOutQuint(float t)
        { 
            --t;
            return 1.0f + t*t*t*t*t; 
        }

        // acceleration until halfway, then deceleration 
        static float easeInOutQuint(float t)
        {
            if (t < 0.5f)
            {
                return 16.0f * t*t*t*t*t;
            }
            else
            {
                --t;
                return 1.0f + 16.0f * t*t*t*t*t;
            }
        }

        static float easeOutCirc(float t)
        {
            --t;
            return glm::sqrt(1.0f - t*t);
        };

        static float easeOutBounce(float t)
        {
            if (t < (1.0f / 2.75f))
            {
                return 7.5625f * t * t;
            }
            else if (t < (2.0f / 2.75f))
            {
                t -= (1.5f / 2.75f);
                return 7.5625f * t * t + 0.75f;
            }
            else if (t < (2.5f / 2.75f))
            {
                t -= (2.25f / 2.75f);
                return 7.5625f * t * t + 0.9375f;
            }
            else
            {
                t -= (2.625f / 2.75f);
                return 7.5625f * t * t + 0.984375f;
            }
        }

        static float easeOutElastic(float t)
        {
            float s = 1.70158f;
            float p = 0.0f;
            float a = 1.0f;
            
            if (t == 0.0f)
                return 0.0f;
            
            if (t == 1.0f)
                return 1.0f;
            
            if (!p)
                p = 0.3f;

            if (a < 1.0f)
            {
                a = 1.0f;
                s = p / 4.0f;
            }
            else
            {
                s = p / (2.0f * phi::PI) * std::asin(1.0f / a);
            }

            return a * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * phi::PI) / p) + 1.0f;
        }

        static float easeOutBack(float t, float s)
        {
            --t;
            return t * t * ((s + 1.0f) * t + s) + 1.0f;
        }

        static float easeOutBackDefault(float t)
        {
            return easeOutBack(t, 1.70158f);
        }

        static float easeBounceBack(float t)
        {
            return static_cast<float>(glm::sin(t * phi::PI));
        }

        static float easeRubberBack(float t)
        {
            return static_cast<float>(glm::sin(glm::pow(t, 1.0f / 2.0f) * phi::PI));
        }
    };
}