#ifndef _PHI_FONT_H_
#define _PHI_FONT_H_

#include <SDL\SDL_ttf.h>
#include <string>
#include "rendering.h"
#include "resource.h"

namespace phi
{
    class font :
        public resource
    {
    private:
        TTF_Font* _ttfFont;
        int _size;
    public:
        RENDERING_API font(std::string name, std::string path, int size);
        RENDERING_API ~font();

		RENDERING_API TTF_Font* getTtfFont() const { return _ttfFont; }
    };
}

#endif