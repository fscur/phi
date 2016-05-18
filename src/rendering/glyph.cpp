#include <precompiled.h>
#include "glyph.h"

namespace phi
{
    glyph::~glyph()
    {
        free(data);
    }
}