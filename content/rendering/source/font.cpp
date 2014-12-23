#include "font.h"

namespace phi
{
    font::font(std::string name, int size)
    {
        _ttfFont = resourceManager::get()->getFont(name, size);
    }

    font::~font()
	{
    }

    void font::setName(std::string name)
    {
        _name = name;
        _ttfFont = resourceManager::get()->getFont(name, _size);
    }
    
    void font::setSize(int size)
    {
        _size = size;
        _ttfFont = resourceManager::get()->getFont(_name, size);
    }
}