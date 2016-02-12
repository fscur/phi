#include "texture.h"

#include <core/globals.h>
#include <io/path.h>

#include <stdio.h>

namespace phi
{
    texture* texture::_defaultAlbedo = nullptr;
    texture* texture::_defaultNormal = nullptr;
    texture* texture::_defaultSpecular = nullptr;
    texture* texture::_defaultEmissive = nullptr;

    texture* texture::defaultAlbedo()
    {
        if (_defaultAlbedo == nullptr)
            _defaultAlbedo = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });

        return _defaultAlbedo;
    }

    texture* texture::defaultNormal()
    {
        if (_defaultNormal == nullptr)
            _defaultNormal = createDefault(new byte[4]{ 0xFF, 0x80, 0xFF, 0xFF });

        return _defaultNormal;
    }

    texture* texture::defaultSpecular()
    {
        if (_defaultSpecular == nullptr)
            _defaultSpecular = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });

        return _defaultSpecular;
    }

    texture* texture::defaultEmissive()
    {
        if (_defaultEmissive == nullptr)
            _defaultEmissive = createDefault(new byte[4]{ 0xFF, 0xFF, 0xFF, 0xFF });

        return _defaultEmissive;
    }

    texture* texture::createDefault(byte* data)
    {
        return new texture(
            1, 
            1, 
            GL_TEXTURE_2D,
            GL_RGBA8, 
            GL_BGRA, 
            GL_UNSIGNED_BYTE, 
            data);
    }
}