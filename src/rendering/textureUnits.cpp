#include <precompiled.h>
#include "textureUnits.h"
#include <core\invalidInitializationException.h>

namespace phi
{
    unordered_map<GLint, bool> textureUnits::_usedUnits;
    vector<GLint> textureUnits::units;
    GLint textureUnits::_maxTextureUnits = -1;

    void textureUnits::initialize()
    {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);

        for (auto i = 0; i < _maxTextureUnits; ++i)
        {
            _usedUnits[i] = false;
            units.push_back(i);
        }
    }

    GLint textureUnits::get()
    {
        for (auto i = 0; i < _maxTextureUnits; ++i)
        {
            if (!_usedUnits[i])
            {
                _usedUnits[i] = true;
                return i;
            }
        }

        throw invalidInitializationException("textureUnits not initialized.");
    }

    void textureUnits::clear(GLint unit)
    {
        _usedUnits[unit] = false;
    }
}