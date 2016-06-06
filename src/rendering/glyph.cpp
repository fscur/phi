#include <precompiled.h>
#include "glyph.h"

namespace phi
{
    glyph::glyph(phi::image* image) :
        image(image)
    {
    }

    glyph::~glyph()
    {
        //safeDelete(image); //TODO: fix this
    }
}