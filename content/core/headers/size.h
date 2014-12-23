#ifndef _PHI_SIZE_H_
#define _PHI_SIZE_H_

namespace phi
{
    template <typename T>
    class size
    {
	private:
		T _width;
        T _height;
        T _depth;

	public:
		const T &width;
        const T &height;
        const T &depth;

    public:
        size(T w = (T)0, T h = (T)0, T d = (T)0) :
			_width(w),
			_height(h),
			_depth(d),
			width(_width),
			height(_height),
			depth(_depth)
        {
        }

        ~size(){}

		void operator=(const size<T> &s)
		{
			_width = s.width;
			_height = s.height;
			_depth = s.depth;
		}
    };
}

#endif 