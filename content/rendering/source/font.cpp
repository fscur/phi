#include "font.h"
#include "resource.h"

namespace phi
{
    font::font(std::string name, std::string path, int size) :
        resource(name, path)
    {
        _ttfFont = TTF_OpenFont(path.c_str(), size);
        _size = size;
    }

    font::~font()
	{
    }
}