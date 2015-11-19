#include "color.h"

namespace phi
{
    color::color(float r, float g, float b, float a) :
            r(r), g(g), b(b), a(a)
    {}

    color color::transparent = color::fromRGBA(0.0f, 0.0f, 0.0f, 0.0f);
    //shades of gray
    color color::black = color::fromRGBA(0.0f, 0.0f, 0.0f, 1.0f);
    color color::gray = color::fromRGBA(0.7421875f, 0.7421875f, 0.7421875f, 1.0f);
    color color::white = color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    
    //primary
    color color::red = color::fromRGBA(1.0f, 0.0f, 0.0f, 1.0f);
    color color::green = color::fromRGBA(0.0f, 1.0f, 0.0f, 1.0f);
    color color::blue = color::fromRGBA(0.0f, 0.0f, 1.0f, 1.0f);
    
    //secondary
    color color::yellow = color::fromRGBA(1.0f, 1.0f, 0.0f, 1.0f);
    color color::magenta = color::fromRGBA(1.0f, 0.0f, 1.0f, 1.0f);
    color color::cyan = color::fromRGBA(0.0f, 1.0f, 1.0f, 1.0f);

    //shades of red
    color color::orange = color::fromRGBA(1.0f, 0.5f, 0.0f, 1.0f);

}