#ifndef _PHI_COLOR_H_
#define _PHI_COLOR_H_

#include "core.h"

namespace phi
{
    struct color
    {
    public:
        float r;
        float g;
        float b;
        float a;

    public:
        CORE_API color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    public :
        CORE_API static color fromRGBA(float r, float g, float b, float a)
        {
            return color(r, g, b, a);
        }

        CORE_API friend bool operator==(const color& a, const color& b)
        {
            return 
                a.r == b.r && 
                a.g == b.g &&
                a.b == b.g &&
				a.a == b.a;
        }

        CORE_API friend bool operator!=(const color& a, const color& b)
        {
            return !(a == b);
        }

        CORE_API static color transparent;
        CORE_API static color black;
        CORE_API static color gray;
        CORE_API static color white;
        CORE_API static color red;
        CORE_API static color green;
        CORE_API static color blue;
        CORE_API static color yellow;
        CORE_API static color magenta;
        CORE_API static color cyan;
        CORE_API static color orange;
    };
}
#endif
