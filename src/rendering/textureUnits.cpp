#include <precompiled.h>
#include "textureUnits.h"
#include <core\invalidInitializationException.h>

namespace phi
{
    map<GLint, bool> textureUnits::_units;
    GLint textureUnits::_maxTextureUnits = -1;

    void textureUnits::initialize()
    {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);

        for (auto i = 0; i < _maxTextureUnits; ++i)
            _units[i] = false;
    }

    GLint textureUnits::get()
    {
        for (auto i = 0; i < _maxTextureUnits; ++i)
        {
            if (!_units[i])
            {
                _units[i] = true;
                return i;
            }
        }

        throw invalidInitializationException("textureUnits not initialized.");
    }

    void textureUnits::clear(GLint unit)
    {
        _units[unit] = false;
    }
}