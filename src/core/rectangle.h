#pragma once

namespace phi
{
    template <typename T>
    struct rectangle
    {
        T x;
        T y;
        T w;
        T h;

        rectangle(
            T x = 0,
            T y = 0,
            T w = 0,
            T h = 0) :
            x(x),
            y(y),
            w(w),
            h(h)
        {
        }

        bool contains(rectangle r)
        {
            if (w < r.w || h < r.h)
                return false;

            return true;
        }
    };
}