#ifndef _PHI_FONT_H_
#define _PHI_FONT_H_

#include "rendering.h"
#include "resourceManager.h"
#include <string>

namespace phi
{
    class font
    {
    private:
        TTF_Font* _ttfFont;
        std::string _name;
        int _size;
    public:
        RENDERING_API font(std::string name, int size);
        RENDERING_API ~font();

		RENDERING_API TTF_Font* getTtfFont() const { return _ttfFont; }
        RENDERING_API void setName(std::string name);
        RENDERING_API void setSize(int size);
    };
}

#endif