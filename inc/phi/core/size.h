#ifndef _PHI_SIZE_H_
#define _PHI_SIZE_H_

namespace phi
{
    template <typename T>
    class tsize
    {
    private:
        T _width;
        T _height;
        T _depth;

    public:
        const T &w;
        const T &h;
        const T &d;

    public:
        tsize(T width = (T)0, T height = (T)0, T depth = (T)0) :
            _width(width),
            _height(height),
            _depth(depth),
            w(_width),
            h(_height),
            d(_depth)
        {
        }

        ~tsize() {}

        void operator=(const tsize<T> &s)
        {
            _width = s.w;
            _height = s.h;
            _depth = s.d;
        }
    };

    using sizeui = tsize<unsigned int>;
    using sizeul = tsize<unsigned long>;
    using sizef = tsize<float>;
    using sized = tsize<double>;
}

#endif 