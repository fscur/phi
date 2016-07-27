#pragma once
#include <phi.h>

namespace phi
{
    class easingFunctions
    {
    public:
        // no easing, no acceleration
        static double linear(double t) { return t; }

        // accelerating from zero velocity
        static double easeInQuad(double t) { return t*t; }

        // decelerating to zero velocity
        static double easeOutQuad(double t) { return t * (2.0 - t); }

        // acceleration until halfway, then deceleration
        static double easeInOutQuad(double t) 
        { 
            return t < 0.5 ? 
                2.0 * t*t : 
                -1.0 + (4.0 - 2.0 * t) * t; 
        }

        // accelerating from zero velocity
        static double easeInCubic(double t) { return t*t*t; }

        // decelerating to zero velocity
        static double easeOutCubic(double t) 
        { 
            --t;
            return t*t*t + 1.0; 
        }

        // acceleration until halfway, then deceleration
        static double easeInOutCubic(double t) 
        { 
            return t < 0.5 ? 
                4.0 * t*t*t : 
                (t - 1.0) * (2.0 * t - 2.0) * (2.0 * t - 2.0) + 1.0; 
        }

        // accelerating from zero velocity 
        static double easeInQuart(double t) { return t*t*t*t; }

        // decelerating to zero velocity 
        static double easeOutQuart(double t) 
        {
            --t;
            return 1.0 - t*t*t*t; 
        }

        // acceleration until halfway, then deceleration
        static double easeInOutQuart(double t) 
        {
            if (t < 0.5)
            {
                return 8.0 * t*t*t*t;
            }
            else
            {
                --t;
                return 1.0 - 8.0 * t*t*t*t;
            }
        }

        // accelerating from zero velocity
        static double easeInQuint(double t) { return t*t*t*t*t; }

        // decelerating to zero velocity
        static double easeOutQuint(double t)
        { 
            --t;
            return 1.0 + t*t*t*t*t; 
        }

        // acceleration until halfway, then deceleration 
        static double easeInOutQuint(double t)
        {
            if (t < 0.5)
            {
                return 16.0 * t*t*t*t*t;
            }
            else
            {
                --t;
                return 1.0 + 16.0 * t*t*t*t*t;
            }
        }

        static double easeOutCirc(double t)
        {
            --t;
            return glm::sqrt(1.0 - t*t);
        };

        static double easeOutBounce(double t)
        {
            if (t < (1.0 / 2.75))
            {
                return 7.5625 * t * t;
            }
            else if (t < (2.0 / 2.75))
            {
                t -= (1.5 / 2.75);
                return 7.5625 * t * t + 0.75;
            }
            else if (t < (2.5 / 2.75))
            {
                t -= (2.25 / 2.75);
                return 7.5625 * t * t + 0.9375;
            }
            else
            {
                t -= (2.625 / 2.75);
                return 7.5625 * t * t + 0.984375;
            }
        }

        static double easeOutElastic(double t)
        {
            double s = 1.70158;
            double p = 0.0;
            double a = 1.0;
            
            if (t == 0.0)
                return 0.0;
            
            if (t == 1.0)
                return 1.0;
            
            if (!p)
                p = 0.3f;

            if (a < 1.0)
            {
                a = 1.0;
                s = p / 4.0;
            }
            else
            {
                s = p / (2.0 * phi::PI) * std::asin(1.0 / a);
            }

            return a * std::pow(2.0, -10.0 * t) * std::sin((t - s) * (2.0 * phi::PI) / p) + 1.0;
        }

        static double easeOutBack(double t, double s)
        {
            --t;
            return t * t * ((s + 1.0) * t + s) + 1.0;
        }

        static double easeOutBackDefault(double t)
        {
            return easeOutBack(t, 1.70158);
        }

        static double easeBounceBack(double t)
        {
            return static_cast<double>(glm::sin(t * phi::PI));
        }

        static double easeRubberBack(double t)
        {
            return static_cast<double>(glm::sin(glm::pow(t, 1.0 / 2.0) * phi::PI));
        }
    };
}