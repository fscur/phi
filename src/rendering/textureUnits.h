#pragma once
#include <phi.h>

namespace phi
{
    class textureUnits
    {
    private:
        static map<GLint, bool> _units;
        static GLint _maxTextureUnits;

    public:
        static void initialize();
        static GLint get();
        static void clear(GLint);
    };
}
