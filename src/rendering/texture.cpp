#include "texture.h"

#include <core/globals.h>
#include <io/path.h>

#include <stdio.h>
#include "gl.h"

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
        if (phi::gl::currentState->useSparseTextures)
        {
            auto x = 128;
            auto y = 128;
            auto d = new byte[x * y * 4];

            for (size_t i = 0; i < x; ++i)
            {
                for (size_t j = 0; j < y * 4; j += 4)
                {
                    auto index = (j + y * i * 4);

                    d[index + 0] = data[0];
                    d[index + 1] = data[1];
                    d[index + 2] = data[2];
                    d[index + 3] = data[3];
                }
            }

            return new texture(
                x,
                y,
                GL_TEXTURE_2D,
                GL_RGBA8,
                GL_BGRA,
                GL_UNSIGNED_BYTE,
                d);
        }
        else
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