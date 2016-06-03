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
            w(w),
            h(h),
            d(1)
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

    //using sizeui = tsize<size_t>;
    using sizeui = tsize<uint>;
    using sizeul = tsize<ulong>;
    using sizef = tsize<float>;
    using sized = tsize<double>;
}
