#ifndef _PHI_SIZE_H_
#define _PHI_SIZE_H_

namespace phi
{
    template <typename T>
    struct tsize
    {
    public:
        T &w;
        T &h;
        T &d;

    public:
        tsize(T w = (T)0, T h = (T)0, T d = (T)0) :
            w(w),
            h(h),
            d(d)
        {
        }

        ~tsize(){}

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

#endif 