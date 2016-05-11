#pragma once

namespace phi
{
    struct rectangle
    {
        int x;
        int y;
        int w;
        int h;

        rectangle(
            int x = 0,
            int y = 0,
            int w = 0,
            int h = 0) :
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