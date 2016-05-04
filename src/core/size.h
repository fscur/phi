#pragma once
#include <phi.h>

namespace phi
{
    template <typename T>
    struct tsize
    {
    public:
        T w;
        T h;
        T d;

    public:
        tsize(T s) :
            w(s),
            h(s),
            d(s)
        {
        }

        tsize(T w, T h) :
            w(s),
            h(h),
            d(1.0f)
        {
        }

        tsize(T w, T h, T d) :
            w(w),
            h(h),
            d(d)
        {
        }

        ~tsize() {}

        void operator=(const tsize<T> &s)
        {
            w = s.w;
            h = s.h;
            d = s.d;
        }
    };

    using sizeui = tsize<unsigned int>;
    using sizeul = tsize<unsigned long>;
    using sizef = tsize<float>;
    using sized = tsize<double>;
}
